<?hh // strict

namespace Waffle\Container\Argument;

use type Waffle\Container\ContainerAwareInterface;
use type ReflectionFunctionAbstract;

interface ArgumentResolverInterface extends ContainerAwareInterface
{
    /**
     * Resolve an array of arguments to their concrete implementations.
     */
    public function resolveArguments(Vector<mixed> $arguments): Vector<mixed>;

    /**
     * Resolves the correct arguments to be passed to a method.
     */
    public function reflectArguments(
        ReflectionFunctionAbstract $method, Map<string, mixed> $args
    ): Vector<mixed>;
}
