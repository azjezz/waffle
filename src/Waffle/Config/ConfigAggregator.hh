<?hh // strict

namespace Waffle\Config;

use namespace HH\Lib\C;
use namespace HH\Lib\Str;
use function date;
use function file_exists;
use function file_put_contents;
use function get_class;
use function var_export;

class ConfigAggregator
{
    const string ENABLE_CACHE = '__CONFIG_CACHE_ENABLED';

    const string CACHE_TEMPLATE = <<< 'TEMPLATE'

TEMPLATE;

    private dict<arraykey, mixed> $config;

    public function __construct(
        vec<Provider\ProviderInterface> $providers = vec[],
        ?string $cacheConfigFile = null
    ) {
        $config = $this->loadConfigFromCache($cacheConfigFile);
        if ($config is nonnull) {
            $this->config = $config;
            return;
        }
        $this->config = $this->loadConfigFromProviders($providers);
        $this->cacheConfig($this->config, $cacheConfigFile);
    }

    public function get(): dict<arraykey, mixed>
    {
        return $this->config;
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
     *
     * @param array $config
     * @param null|string $cachedConfigFile
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
}