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
   * Returns an ImmVector containing, as values, the keys of the current `Configuration`.
   *
   * @return - an ImmVector containing, as values, the keys of the current
   *           `Configuration`.
   */
  public function keys(): ImmVector<string>;

  /**
   * Provides the number of elements in the current `Configuration`.
   *
   * @return - The number of elements in current `Configuration`.
   */
  public function count(): int;

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
  public function at(string $k): mixed;

  /**
   * Returns the value at the specified key in the current `Configuration`.
   *
   * If the key is not present, null is returned. If you would rather have an
   * exception thrown when a key is not present, then use `at()`.
   *
   * @param $k - the key from which to retrieve the value.
   *
   * @return - The value at the specified key; or `null` if the key does not
   *           exist.
   */
  public function get(string $k): mixed;

  /**
   * Determines if the specified key is in the current `Configuration`.
   *
   * @param $k - The key to check.
   *
   * @return - `true` if the specified key is present in the current `Configuration`;
   *           `false` otherwise.
   */
  public function contains(string $k): bool;

  /**
   * Returns an iterator that points to beginning of the current `Configuration`.
   *
   * @return - A `KeyedIterator` that allows you to traverse the current
   *           `Configuration`.
   */
  public function getIterator(): KeyedIterator<string, mixed>;

  /**
   * Returns an `Iterable` view of the current `Configuration`.
   *
   * The `Iterable` returned is one that produces the key/values from the
   * current `Configuration`.
   *
   * @return - The `Iterable` view of the current `Configuration`.
   */
  public function items(): Iterable<Pair<string, mixed>>;
}
