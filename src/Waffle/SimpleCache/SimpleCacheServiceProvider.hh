<?hh // strict

namespace Waffle\SimpleCache;

use namespace HH\Lib\Str;
use type Waffle\Contract\SimpleCache\CacheInterface;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use function is_object;
use function get_class;
use function gettype;

class SimpleCacheServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        CacheInterface::class,
    ];

    <<__Override>>
    public function register(): void
    {
        $this->share(
            CacheInterface::class,
            (): CacheInterface ==> {
                $container = $this->getContainer();

                if (!$container->has(CacheItemPoolInterface::class)) {
                    throw Exception\MissingDependencyException::forService(CacheItemPoolInterface::class);
                }

                $pool = $this->getContainer()->get(CacheItemPoolInterface::class);

                if (!$pool is CacheItemPoolInterface) {
                    throw new Exception\InvalidArgumentException(Str\format(
                        'Expected  "%s" service to resolve to an instance of %s, %s given.',
                        CacheItemPoolInterface::class,
                        CacheItemPoolInterface::class,
                        is_object($pool) ? get_class($pool) : gettype($pool)
                    ));
                }

                return new Cache($pool);
            }
        );
    }
}
