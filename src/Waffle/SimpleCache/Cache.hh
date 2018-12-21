<?hh // strict

namespace Waffle\SimpleCache;

use type Waffle\Contract\SimpleCache\CacheInterface;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\Contract\Cache\CacheExceptionInterface as PoolException;
use type Waffle\Contract\Cache\InvalidArgumentExceptionInterface as InvalidArgumentPoolException;
use type Waffle\Contract\Service\ResetInterface;
use type DateInterval;
use type Throwable;

class Cache implements CacheInterface, ResetInterface
{
    public function __construct(
        protected CacheItemPoolInterface $pool
    ) {}

    /**
     * Fetches a value from the cache.
     */
    public function get(string $key, mixed $default = null): mixed
    {
        $this->box((): mixed ==> {
            $item = $this->pool->getItem($key);
            return $item->isHit() ? $item->get() : $default;
        });
    }

    /**
     * Persists data in the cache, uniquely referenced by a key with an optional expiration TTL time.
     */
    public function set(string $key, mixed $value, ?DateInterval $ttl = null): bool
    {
        return $this->box((): bool ==> $this->pool->save(
            $this->pool->getItem($key)->set($value)->expiresAfter($ttl)
        ));
    }

    /**
     * Delete an item from the cache by its unique key.
     */
    public function delete(string $key): bool
    {
        return $this->box((): bool ==> $this->pool->deleteItem($key));
    }

    /**
     * Wipes clean the entire cache's keys.
     */
    public function clear(): bool
    {
        return $this->box((): bool ==> $this->pool->clear());
    }

    /**
     * Obtains multiple cache items by their unique keys.
     */
    public function getMultiple(Container<string> $keys,mixed $default = null): KeyedContainer<string, mixed>
    {
        return $this->box((): KeyedContainer<string, mixed> ==> {
            $items = $this->pool->getItems($keys);

            $values = dict[];
            foreach ($items as $key => $item) {
                $values[$key] = $item->isHit() ? $item->get() : $default;
            }

            return $values;
        });
    }

    /**
     * Persists a set of key => value pairs in the cache, with an optional TTL.
     */
    public function setMultiple(KeyedContainer<string, mixed> $values, ?DateInterval $ttl = null): bool
    {
        return $this->box((): bool ==> {
            $items = vec[];
            foreach ($values as $key => $value) {
                $items[] = $this->pool->getItem($key)->set($value)->expiresAfter($ttl);
            }

            $ok = true;
            foreach ($items as $item) {
                $ok = $this->pool->saveDeferred($item) && $ok;
            }

            return $this->pool->commit() && $ok;
        });
    }

    /**
     * Deletes multiple cache items in a single operation.
     */
    public function deleteMultiple(Container<string> $keys): bool
    {
        return $this->box((): bool ==> $this->pool->deleteItems($keys));
    }

    /**
     * Determines whether an item is present in the cache.
     */
    public function has(string $key): bool
    {
        return $this->box((): bool ==> $this->pool->hasItem($key));
    }

    public function reset(): void
    {
        $this->box((): void ==> {
            $this->pool->reset();
        });
    }

    protected function box<T>((function(): T) $call): T
    {
        try {
            return $call();
        } catch(Throwable $e) {
            if ($e is InvalidArgumentPoolException) {
                $e = new Exception\InvalidArgumentException($e->getMessage(), $e->getCode(), $e);
            } elseif ($e is PoolException) {
                $e = new Exception\CacheException($e->getMessage(), $e->getCode(), $e);
            }

            throw $e;
        }
    }
}
