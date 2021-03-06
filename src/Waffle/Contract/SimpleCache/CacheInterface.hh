<?hh // strict

namespace Waffle\Contract\SimpleCache;

use type DateInterval;

interface CacheInterface
{
    /**
     * Fetches a value from the cache.
     *
     * @param string $key     The unique key of this item in the cache.
     * @param mixed  $default Default value to return if the key does not exist.
     *
     * @return mixed The value of the item from the cache, or $default in case of cache miss.
     *
     * @throws InvalidArgumentExceptionInterface
     *   MUST be thrown if the $key string is not a legal value.
     */
    public function get(string $key, mixed $default = null): mixed;

    /**
     * Persists data in the cache, uniquely referenced by a key with an optional expiration TTL time.
     *
     * @param string                    $key   The key of the item to store.
     * @param mixed                     $value The value of the item to store, must be serializable.
     * @param null|DateInterval         $ttl   Optional. The TTL value of this item. If no value is sent and
     *                                         the driver supports TTL then the library may set a default value
     *                                         for it or let the driver take care of that.
     *
     * @return bool True on success and false on failure.
     *
     * @throws InvalidArgumentExceptionInterface
     *   MUST be thrown if the $key string is not a legal value.
     */
    public function set(string $key, mixed $value, ?DateInterval $ttl = null): bool;

    /**
     * Delete an item from the cache by its unique key.
     *
     * @param string $key The unique cache key of the item to delete.
     *
     * @return bool True if the item was successfully removed. False if there was an error.
     *
     * @throws InvalidArgumentExceptionInterface
     *   MUST be thrown if the $key string is not a legal value.
     */
    public function delete(string $key): bool;

    /**
     * Wipes clean the entire cache's keys.
     *
     * @return bool True on success and false on failure.
     */
    public function clear(): bool;

    /**
     * Obtains multiple cache items by their unique keys.
     *
     * @param iterable $keys    A list of keys that can obtained in a single operation.
     * @param mixed    $default Default value to return for keys that do not exist.
     *
     * @return KeyedContainer<string, mixed> A list of key => value pairs. Cache keys that do not exist or are stale will have $default as value.
     */
    public function getMultiple(Container<string> $keys,mixed $default = null): KeyedContainer<string, mixed>;

    /**
     * Persists a set of key => value pairs in the cache, with an optional TTL.
     *
     * @param KeyedContainer<string, mixed> $values A list of key => value pairs for a multiple-set operation.
     * @param null|DateInterval             $ttl    Optional. The TTL value of this item. If no value is sent and
     *                                              the driver supports TTL then the library may set a default value
     *                                              for it or let the driver take care of that.
     *
     * @return bool True on success and false on failure.
     */
    public function setMultiple(KeyedContainer<string, mixed> $values,?DateInterval $ttl = null): bool;

    /**
     * Deletes multiple cache items in a single operation.
     *
     * @param Container<string> $keys A list of string-based keys to be deleted.
     *
     * @return bool True if the items were successfully removed. False if there was an error.
     */
    public function deleteMultiple(Container<string> $keys): bool;

    /**
     * Determines whether an item is present in the cache.
     *
     * NOTE: It is recommended that has() is only to be used for cache warming type purposes
     * and not to be used within your live applications operations for get/set, as this method
     * is subject to a race condition where your has() will return true and immediately after,
     * another script can remove it making the state of your app out of date.
     *
     * @param string $key The cache item key.
     *
     * @return bool
     *
     * @throws InvalidArgumentExceptionInterface
     *   MUST be thrown if the $key string is not a legal value.
     */
    public function has(string $key): bool;
}
