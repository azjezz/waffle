<?hh // strict

namespace Waffle\Container\Argument;

use type Waffle\Container\ContainerAwareInterface;
use type ReflectionFunctionAbstract;

interface ArgumentResolverInterface extends ContainerAwareInterface
{
    /**
     * Resolve a vector of arguments to their concrete implementations.
     */
    public function resolveArguments(vec<mixed> $arguments): vec<mixed>;

    /**
     * Resolves the correct arguments to be passed to a method.
     */
    public function reflectArguments(
        ReflectionFunctionAbstract $method, dict<string, mixed> $args
    ): vec<mixed>;
}
