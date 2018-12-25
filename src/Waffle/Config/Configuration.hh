<?hh // strict

namespace Waffle\Config;

use namespace HH\Lib\C;
use namespace HH\Lib\Str;
use type Waffle\Contract\Config\ConfigurationInterface;
use function date;
use function file_exists;
use function file_put_contents;
use function get_class;
use function var_export;

class Configuration implements ConfigurationInterface
{
    private ImmMap<arraykey, mixed> $items;

    const string ENABLE_CACHE = '__CONFIG_CACHE_ENABLED';

    public function __construct(
        vec<Provider\ProviderInterface> $providers = vec[],
        ?string $cacheConfigFile = null
    ) {
        $config = $this->loadConfigFromCache($cacheConfigFile);
        if ($config is nonnull) {
            $this->items = new ImmMap($config);
            return;
        }
        $config = $this->loadConfigFromProviders($providers);
        $this->items = new ImmMap($config);
        $this->cacheConfig($config, $cacheConfigFile);
    }

   /**
     * Iterate providers, merging config from each with the previous.
     */
    private function loadConfigFromProviders(vec<Provider\ProviderInterface> $providers): dict<arraykey, mixed>
    {
        $mergedConfig = dict[];
        foreach ($providers as $provider) {
            $config = $provider->load();
            $mergedConfig = __Private\merge_recursive($mergedConfig, $config);
        }
        return $mergedConfig;
    }

    /**
     * Attempt to load the configuration from a cache file.
     */
    private function loadConfigFromCache(?string $cachedConfigFile): ?dict<arraykey, mixed>
    {
        if (null === $cachedConfigFile) {
            return null;
        }
        if (! file_exists($cachedConfigFile)) {
            return null;
        }
        return require $cachedConfigFile;
    }

    /**
     * Attempt to cache discovered configuration.
     */
    private function cacheConfig(dict<arraykey, mixed> $config, ?string $cachedConfigFile): void
    {
        if (null === $cachedConfigFile) {
            return;
        }

        if (!C\contains_key($config, static::ENABLE_CACHE)) {
            return;
        }

        $enableCache = $config[static::ENABLE_CACHE];

        if ($enableCache !== true) {
            return;
        }

        file_put_contents($cachedConfigFile, Str\format(
            '<?hh
/**
 * This configuration cache file was generated by %s
 * as %s
 */

return %s;',
            get_class($this),
            date('c'),
            var_export($config, true)
        ));
    }

    /**
    * Returns an ImmVector containing the values of the current `Configuration`.
    *
    * @return - an ImmVector containing the values of the current `Configuration`.
    */
    public function values(): ImmVector<mixed>
    {
        return $this->items->values();
    }

    /**
    * Returns an ImmVector containing, as values, the keys of the current `Configuration`.
    *
    * @return - an ImmVector containing, as values, the keys of the current
    *           `Configuration`.
    */
    public function keys(): ImmVector<arraykey>
    {
        return $this->items->keys();
    }

    /**
    * Provides the number of elements in the current `Configuration`.
    *
    * @return - The number of elements in current `Configuration`.
    */
    public function count(): int
    {
        return $this->items->count();
    }

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
    public function at(arraykey $k): mixed
    {
        return $this->items->at($k);
    }

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
    public function get(arraykey $k, mixed $default = null): mixed
    {
        if (!$this->contains($k)) {
            return $default;
        }
        return $this->items->get($k);
    }

    /**
    * Determines if the specified key is in the current `Configuration`.
    *
    * @param $k - The key to check.
    *
    * @return - `true` if the specified key is present in the current `Configuration`;
    *           `false` otherwise.
    */
    public function contains(arraykey $k): bool
    {
        return $this->items->contains($k);
    }

    /**
    * Returns an `Iterable` view of the current `Configuration`.
    *
    * The `Iterable` returned is one that produces the key/values from the
    * current `Configuration`.
    *
    * @return - The `Iterable` view of the current `Configuration`.
    */
    public function items(): Iterable<Pair<arraykey, mixed>>
    {
        return $this->items->items();
    }
}
