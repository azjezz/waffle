<?hh // strict

namespace Waffle\Container;

use namespace HH\Lib\Str;
use type Waffle\Container\Argument\ArgumentResolverInterface;
use type Waffle\Container\Argument\ArgumentResolverTrait;
use type Waffle\Container\Exception\NotFoundException;
use type Waffle\Contract\Container\ContainerInterface;
use type ReflectionClass;
use type ReflectionFunction;
use type ReflectionMethod;
use function is_object;
use function class_exists;
use function explode;
use function is_array;

class ReflectionContainer implements ArgumentResolverInterface, ContainerInterface
{
    use ArgumentResolverTrait;
    use ContainerAwareTrait;

    /**
    * Cache of reslutions.
    */
    protected Map<string, mixed> $cache;

    public function __construct(protected bool $cacheResolutions = false)
    {
        $this->cache = Map {};
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $id, Map<string, mixed> $args = Map {}): mixed
    {
        if ($this->cacheResolutions === true && $this->cache->contains($id)) {
            return $this->cache->get($id);
        }

        if (! $this->has($id)) {
            throw new NotFoundException(
                Str\format('Alias (%s) is not an existing class and therefore cannot be resolved', $id)
            );
        }

        $reflector = new ReflectionClass($id);
        $construct = $reflector->getConstructor();

        $resolution = null === $construct
            ? $reflector->newInstance()
           : $reflector->newInstanceArgs(
                $this->reflectArguments($construct, $args)
              )
        ;

        if ($this->cacheResolutions === true) {
            $this->cache->set($id, $resolution);
        }

        return $resolution;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $id): bool
    {
        return class_exists($id);
    }

    /**
     * Invoke a callable via the container.
     *
     * @param callable $callable
     * @param array    $args
     *
     * @return mixed
     */
    public function call(mixed $callable, Map<string, mixed> $args = Map {}): mixed
    {
        if (($callable is string) && Str\search($callable, '::') !== null) {
            $callable = explode('::', $callable);
        }

        if (is_array($callable)) {
            if ($callable[0] is string) {
                $callable[0] = $this->getContainer()->get($callable[0]);
            }

            $reflection = new ReflectionMethod($callable[0], $callable[1]);

            if ($reflection->isStatic()) {
                $callable[0] = null;
            }

            return $reflection->invokeArgs(
                $callable[0],
                $this->reflectArguments(
                    $reflection,
                    $args
                )->toVArray()
            );
        }

        if (is_object($callable)) {
            $reflection = new ReflectionMethod($callable, '__invoke');

            return $reflection->invokeArgs(
                $callable,
                $this->reflectArguments(
                    $reflection,
                    $args
                )->toVArray()
            );
        }

        $reflection = new ReflectionFunction($callable);

        return $reflection->invokeArgs(
            $this->reflectArguments(
                $reflection,
                $args
            )->toVArray()
        );
    }

    /**
     * Whether the container should default to caching resolutions and returning
     * the cache on following calls.
     *
     * @param boolean $option
     *
     * @return self
     */
    public function cacheResolutions(bool $option = true): ContainerInterface
    {
        $this->cacheResolutions = $option;

        return $this;
    }
}
