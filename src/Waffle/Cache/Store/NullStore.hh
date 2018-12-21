<?hh // strict

namespace Waffle\Cache\Store;

class NullStore implements StoreInterface
{
    /**
     * Persists data in the cache, uniquely referenced by a key with an optional expiration TTL time.
     */
    public function store(string $id, mixed $value, ?num $ttl = null): bool
    {
        return false;
    }

    /**
     * Sets a cache item to be persisted later.
     */
    public function defer(string $id, mixed $value, ?num $ttl = null): bool
    {
        return false;
    }

    /**
     * Determines whether an item is present in the cache.
     */
    public function contains(string $id): bool
    {
        return false;
    }

    /**
     * Delete an item from the cache by its unique key.
     */
    public function delete(string $id): bool
    {
        return false;
    }

    /**
     * Fetches a value from the cache.
     */
    public function get(string $id): mixed
    {
        return null;
    }

    /**
     * Wipes clean the entire cache's keys.
     */
    public function clear(): bool
    {
        return false;
    }

    /**
     * Persists any deferred cache items.
     */
    public function commit(): bool
    {
        return false;
    }
}
