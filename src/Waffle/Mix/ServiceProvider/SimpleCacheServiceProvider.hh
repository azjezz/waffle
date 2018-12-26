<?hh // strict

namespace Waffle\Mix\ServiceProvider;

use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use type Waffle\Contract\SimpleCache\CacheInterface;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\SimpleCache\Cache;

class SimpleCacheServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        CacheInterface::class
    ];

    <<__Override>>
    public function register(): void
    {
        $this->share(CacheInterface::class, Cache::class)
            ->addArgument(CacheItemPoolInterface::class);
    }
}
