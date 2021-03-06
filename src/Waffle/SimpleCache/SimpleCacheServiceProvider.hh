<?hh // strict

namespace Waffle\SimpleCache;

use type Waffle\Contract\SimpleCache\CacheInterface;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;

class SimpleCacheServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        CacheInterface::class,
    ];

    <<__Override>>
    public function register(): void
    {
        $this->share(CacheInterface::class)
            ->addArgument(CacheItemPoolInterface::class);
    }
}
