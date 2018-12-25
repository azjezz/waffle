<?hh // strict

namespace Waffle\Contract\Config;

interface ConfigurationInterface
{
    /**
    * Returns an ImmVector containing the values of the current `Configuration`.
    *
    * @return - an ImmVector containing the values of the current `Configuration`.
    */
    public function values(): ImmVector<mixed>;

    /**
    * Returns the value at the specified key in the current `Configuration`.
    *
    * If the key is not present, an exception is thrown. If you don't want an
    * exception to be thrown, use `get()` instead.
    *
    * `$v = $config->at($k)` is semantically equivalent to `$v = $config[$k]`.
    *
    * @param $k - the key from which to retrieve the value.
    *
    * @return - The value at the specified key; or an exception if the key does
    *           not exist.
    */
    public function at(arraykey $k): mixed;

    /**
    * Returns the value at the specified key in the current `Configuration`.
    *
    * If the key is not present, null is returned. If you would rather have an
    * exception thrown when a key is not present, then use `at()`.
    *
    * @param $k - the key from which to retrieve the value.
    * @param $default - the default value to return if the key does not exist.
    *
    * @return - The value at the specified key; or $default if the key does not
    *           exist.
    */
    public function get(arraykey $k, mixed $default = null): mixed;

    /**
    * Determines if the specified key is in the current `Configuration`.
    *
    * @param $k - The key to check.
    *
    * @return - `true` if the specified key is present in the current `Configuration`;
    *           `false` otherwise.
    */
    public function contains(arraykey $k): bool;

    /**
    * Returns an `Iterable` view of the current `Configuration`.
    *
    * The `Iterable` returned is one that produces the key/values from the
    * current `Configuration`.
    *
    * @return - The `Iterable` view of the current `Configuration`.
    */
    public function items(): Iterable<Pair<arraykey, mixed>>;
}
