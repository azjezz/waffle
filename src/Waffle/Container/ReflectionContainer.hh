<?hh // strict

namespace Waffle\Container;

use Waffle\Container\Argument\ArgumentResolverInterface;
use Waffle\Container\Argument\ArgumentResolverTrait;
use Waffle\Container\Exception\NotFoundException;
use Waffle\Contract\Container\ContainerInterface;
use ReflectionClass;
use ReflectionFunction;
use ReflectionMethod;
use function sprintf;
use function strpos;
use function is_object;
use function is_string;
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
                sprintf('Alias (%s) is not an existing class and therefore cannot be resolved', $id)
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
        if (is_string($callable) && strpos($callable, '::') !== false) {
            $callable = explode('::', $callable);
        }

        if (is_array($callable)) {
            if (is_string($callable[0])) {
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