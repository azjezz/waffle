<?hh // strict

namespace Waffle\Mix\ServiceProvider;

use namespace HH\Lib\C;
use namespace Waffle\Cache\Store;
use namespace Waffle\Cache\Serializer;
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use type Waffle\Contract\Config\ConfigurationInterface;
use type Waffle\Mix\Environment;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\Cache\CacheItemPool;
use type Waffle\Cache\Store\StoreInterface;
use type Waffle\Cache\Serializer\SerializerInterface;
use type Waffle\Container\Argument\RawArgument;
use type Redis;

class CacheServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        CacheItemPoolInterface::class,
        SerializerInterface::class,
        StoreInterface::class,
    ];

    <<__Override>>
    public function register(): void
    {
        $config = $this->getContainer()->get('config') as ConfigurationInterface
                |> $$->get('cache') as ?KeyedContainer<_, _>;
        $config = $config ?? dict[];
        $env = $this->getContainer()->get('env') as Environment;

        $this->share(CacheItemPoolInterface::class, CacheItemPool::class)
            ->addArgument(StoreInterface::class);
        $this->share(SerializerInterface::class, Serializer\DefaultSerializer::class);

        if (C\contains_key($config, 'store')) {
            /* HH_IGNORE_ERROR[4110] */
            $store = $config['store'] as string;
        } else {
            if ($env->debug()) {
                $store = 'null';
            } else {
                $store = $env->dev() ? 'array' : 'apc';
            }
        }

        if (C\contains_key($config, 'namespace')) {
            /* HH_IGNORE_ERROR[4110] */
            $namespace = $config['namespace'] as string;
        } else {
            $namespace = '';
        }

        if (C\contains_key($config, 'default_ttl')) {
            /* HH_IGNORE_ERROR[4110] */
            $defaultTtl = $config['default_ttl'] as int;
        } else {
            $defaultTtl = 0;
        }

        switch ($store) {
            case 'apc':
                $this->share(StoreInterface::class, Store\ApcStore::class)
                    ->addArgument(new RawArgument($namespace))
                    ->addArgument(new RawArgument($defaultTtl))
                    ->addArgument(SerializerInterface::class);
                break;
            case 'array':
                $this->share(StoreInterface::class, Store\ArrayStore::class)
                    ->addArgument(new RawArgument($defaultTtl));
                break;
            case 'redis':
                $this->share(StoreInterface::class, Store\RedisStore::class)
                    ->addArgument(Redis::class)
                    ->addArgument(new RawArgument($namespace))
                    ->addArgument(new RawArgument($defaultTtl))
                    ->addArgument(SerializerInterface::class);
                break;
            case 'null':
            default:
                $this->share(StoreInterface::class, Store\NullStore::class);
                break;
        }
    }
}