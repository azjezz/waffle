<?hh // strict

namespace Waffle\Cache;

use type Waffle\Contract\Cache\CacheExceptionInterface;
use type Waffle\Contract\Cache\InvalidArgumentExceptionInterface;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\Contract\Cache\CacheItemInterface;
use type Waffle\Contract\Log\LoggerInterface;
use type Waffle\Contract\Log\LoggerAwareInterface;
use type Waffle\Contract\Log\LogLevel;
use type Waffle\Contract\Log\NullLogger;
use type Waffle\Contract\Service\ResetInterface;
use type Exception;
use function microtime;

class CacheItemPool implements CacheItemPoolInterface, LoggerAwareInterface
{
    protected LoggerInterface $logger;

    public function __construct(
        protected Store\StoreInterface $store,
        ?LoggerInterface $logger = null
    ) {
        $this->logger = $logger ?? new NullLogger();
    }

    public function setLogger(LoggerInterface $logger): void
    {
        $this->logger = $logger;
    }

    /**
     * Returns a Cache Item representing the specified key.
     *
     * This method must always return a CacheItemInterface object, even in case of
     * a cache miss. It MUST NOT return null.
     *
     * @param string $key
     *   The key for which to return the corresponding Cache Item.
     *
     * @throws InvalidArgumentExceptionInterface
     *   If the $key string is not a legal value a \Waffle\Contract\Cache\InvalidArgumentException
     *   MUST be thrown.
     *
     * @return CacheItemInterface
     *   The corresponding Cache Item.
     */
    public function getItem(string $key): CacheItemInterface
    {
        return $this->box((): CacheItemInterface ==> {
            if ($this->store->contains($key)) {
                $value = $this->store->get($key);
                return new CacheItem($key, $value, true);
            }

            return new CacheItem($key);
        });
    }

    /**
     * Returns a traversable set of cache items.
     *
     * @param Container<string> $keys
     *   An indexed Container of keys of items to retrieve.
     *
     * @throws InvalidArgumentExceptionInterface
     *   If any of the keys in $keys are not a legal value a InvalidArgumentExceptionInterface
     *   MUST be thrown.
     *
     * @return KeyedContainer<string, CacheItemInterface>
     *   A traversable collection of Cache Items keyed by the cache keys of
     *   each item. A Cache item will be returned for each key, even if that
     *   key is not found. However, if no keys are specified then an empty
     *   traversable MUST be returned instead.
     */
    public function getItems(Container<string> $keys = []): KeyedContainer<string, CacheItemInterface>
    {
        return $this->box((): KeyedContainer<string, CacheItemInterface> ==> {
            $items = dict[];
            foreach ($keys as $key) {
                $items[$key] = $this->getItem($key);
            }

            return $items;
        });
    }

    /**
     * Confirms if the cache contains specified cache item.
     *
     * Note: This method MAY avoid retrieving the cached value for performance reasons.
     * This could result in a race condition with CacheItemInterface::get(). To avoid
     * such situation use CacheItemInterface::isHit() instead.
     *
     * @param string $key
     *   The key for which to check existence.
     *
     * @throws InvalidArgumentExceptionInterface
     *   If the $key string is not a legal value a InvalidArgumentExceptionInterface
     *   MUST be thrown.
     *
     * @return bool
     *   True if item exists in the cache, false otherwise.
     */
    public function hasItem(string $key): bool
    {
        return $this->box((): bool ==> {
            return $this->store->contains($key);
        });
    }

    /**
     * Deletes all items in the pool.
     *
     * @return bool
     *   True if the pool was successfully cleared. False if there was an error.
     */
    public function clear(): bool
    {
        return $this->box((): bool ==> $this->store->clear());
    }

    /**
     * Removes the item from the pool.
     *
     * @param string $key
     *   The key to delete.
     *
     * @throws InvalidArgumentExceptionInterface
     *   If the $key string is not a legal value a InvalidArgumentExceptionInterface
     *   MUST be thrown.
     *
     * @return bool
     *   True if the item was successfully removed. False if there was an error.
     */
    public function deleteItem(string $key): bool
    {
        return $this->box((): bool ==> {
            return $this->store->delete($key);
        });
    }

    /**
     * Removes multiple items from the pool.
     *
     * @param string[] $keys
     *   A Container of keys that should be removed from the pool.

     * @throws InvalidArgumentException
     *   If any of the keys in $keys are not a legal value a InvalidArgumentExceptionInterface
     *   MUST be thrown.
     *
     * @return bool
     *   True if the items were successfully removed. False if there was an error.
     */
    public function deleteItems(Container<string> $keys): bool
    {
        return $this->box((): bool ==> {
            $ok = true;
            foreach ($keys as $key) {
                $ok = $this->deleteItem($key) && $ok;
            }

            return $ok;
        });
    }

    /**
     * Persists a cache item immediately.
     *
     * @param CacheItemInterface $item
     *   The cache item to save.
     *
     * @return bool
     *   True if the item was successfully persisted. False if there was an error.
     */
    public function save(CacheItemInterface $item): bool
    {
        return $this->box((): bool ==> {
            $expirationTime = $item->getExpirationTimestamp();
            if ($expirationTime is nonnull) {
                $ttl = $expirationTime - microtime(true);
            } else {
                $ttl = $expirationTime;
            }
            return $this->store->store($item->getKey(), $item->get(), $ttl);
        });
    }

    /**
     * Sets a cache item to be persisted later.
     *
     * @param CacheItemInterface $item
     *   The cache item to save.
     *
     * @return bool
     *   False if the item could not be queued or if a commit was attempted and failed. True otherwise.
     */
    public function saveDeferred(CacheItemInterface $item): bool
    {
        return $this->box((): bool ==> {
            $expirationTime = $item->getExpirationTimestamp();
            if ($expirationTime is nonnull) {
                $ttl = $expirationTime - microtime(true);
            } else {
                $ttl = $expirationTime;
            }

            return $this->store->defer($item->getKey(), $item->get(), $ttl);
        });
    }

    /**
     * Persists any deferred cache items.
     *
     * @return bool
     *   True if all not-yet-saved items were successfully saved or there were none. False otherwise.
     */
    public function commit(): bool
    {
        return $this->box((): bool ==> $this->store->commit());
    }

    /**
     * Reset the store to its intial status.
     */
    public function reset(): void
    {
        $this->box((): void ==> {
            if ($this->store is ResetInterface) {
                $this->store->reset();
            }
        });
    }

    protected function box<T>((function(): T) $fun): T
    {
        try {
            return $fun();
        } catch(Exception $e) {

            $level = LogLevel::ALERT;
            if ($e is InvalidArgumentExceptionInterface) {
                $level = LogLevel::WARNING;
            }

            if (!$e is CacheExceptionInterface) {
                $e = new Exception\CacheException($e->getMessage(), $e->getCode(), $e);
            }

            $this->logger->log($level, 'Cache Exception : {message}', dict[
                'message' => $e->getMessage(),
                'exception' => $e,
            ]);

            throw $e;
        }
    }
}
