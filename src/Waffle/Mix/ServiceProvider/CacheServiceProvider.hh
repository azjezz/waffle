<?hh // strict

namespace Waffle\Mix\ServiceProvider;

use namespace Waffle\Cache\Store;
use namespace Waffle\Cache\Serializer;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\Contract\Config\ConfigurationInterface;
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use type Waffle\Container\Argument\RawArgument;
use type Waffle\Cache\CacheItemPool;
use type Waffle\Cache\Store\StoreInterface;
use type Waffle\Cache\Serializer\SerializerInterface;
use type Waffle\Mix\Environment;
use type Redis;

class CacheServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        CacheItemPoolInterface::class,
        SerializerInterface::class,
        StoreInterface::class,
        Store\RedisStore::class,
        Store\ArrayStore::class,
        Store\NullStore::class,
        Store\ApcStore::class,
    ];

    <<__Override>>
    public function register(): void
    {
        $config = $this->getContainer()->get('config') as ConfigurationInterface
                |> $$->get('cache', shape()) as shape(
                    ?'store' => string,
                    ?'namespace' => string,
                    ?'default_ttl' => int,
                    ...
                );
        $env = $this->getContainer()->get('env') as Environment;

        $this->share(CacheItemPoolInterface::class, CacheItemPool::class)
            ->addArgument(StoreInterface::class);
        $this->share(SerializerInterface::class, Serializer\DefaultSerializer::class);

        $store = $config['store'] ?? ($env->debug() ? 'null' : ($env->dev() ? 'array' : 'apc'));
        $namespace = $config['namespace'] ?? '';
        $defaultTtl = $config['default_ttl'] ?? 0;

        $this->share(Store\ApcStore::class)
                ->addArgument(new RawArgument($namespace))
                ->addArgument(new RawArgument($defaultTtl))
                ->addArgument(SerializerInterface::class);

        $this->share(Store\ArrayStore::class)
                ->addArgument(new RawArgument($defaultTtl));

        $this->share(Store\RedisStore::class)
                ->addArgument(Redis::class)
                ->addArgument(new RawArgument($namespace))
                ->addArgument(new RawArgument($defaultTtl))
                ->addArgument(SerializerInterface::class);

        $this->share(Store\NullStore::class);

        switch ($store) {
            case 'apc':
            case Store\ApcStore::class:
                $store = Store\ApcStore::class;
                break;
            case 'array':
            case Store\ArrayStore::class:
                $store = Store\ArrayStore::class;
                break;
            case 'redis':
            case Store\RedisStore::class:
                $store = Store\RedisStore::class;
                break;
            case 'null':
            case Store\NullStore::class:
                $store = Store\NullStore::class;
                break;
        }

        $this->share(StoreInterface::class, () ==> {
            return $this->getContainer()->get($store);
        });
    }
}
