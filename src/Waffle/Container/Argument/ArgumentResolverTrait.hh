<?hh // strict

namespace Waffle\Container\Argument;

use namespace HH\Lib\Str;
use type Waffle\Container\Exception\ContainerException;
use type Waffle\Container\Exception\NotFoundException;
use type Waffle\Container\ReflectionContainer;
use type Waffle\Contract\Container\ContainerInterface;
use type ReflectionFunctionAbstract;
use type ReflectionParameter;

trait ArgumentResolverTrait
{
    public function resolveArguments(Vector<mixed> $arguments): Vector<mixed>
    {
        $resolved = Vector {};

        foreach ($arguments as $arg) {
            if ($arg instanceof RawArgumentInterface) {
                $resolved->add($arg->getValue());
                continue;
            }

            if ($arg instanceof ClassNameArgumentInterface) {
                $arg = $arg->getValue();
            }

            if (! ($arg is string)) {
                $resolved->add($arg);
                continue;
            }

            $container = null;

            try {
                $container = $this->getContainer();
            } catch (ContainerException $e) {
                if ($this instanceof ReflectionContainer) {
                    $container = $this;
                }
            }


            if (null !== $container && $container->has($arg)) {
                $arg = $container->get($arg);

                if ($arg instanceof RawArgumentInterface) {
                    $arg = $arg->getValue();
                }

                $resolved->add($arg);
                continue;
            }

            $resolved->add($arg);
        }

        return $resolved;
    }

    public function reflectArguments(ReflectionFunctionAbstract $method, Map<string, mixed> $args = Map {}): Vector<mixed>
    {
        $parameters = new Vector<ReflectionParameter>($method->getParameters());
        $arguments = $parameters->map<mixed>(function (ReflectionParameter $param) use ($method, $args) {

            $name   = $param->getName();
            $class  = $param->getClass();

            if ($args->contains($name)) {
                return $args->get($name);
            }

            if (null !== $class) {
                return $class->getName();
            }

            $type = $param->getType();
            if (null !== $type && !$type->isBuiltin()) {
                if ($type->allowsNull()) {
                    return Str\slice((string) $type, 1);
                } else {
                    return (string) $type;
                }
            }

            if ($param->isDefaultValueAvailable()) {
                return $param->getDefaultValue();
            }

            throw new NotFoundException(Str\format(
                'Unable to resolve a value for parameter (%s) in the function/method (%s)',
                $name,
                $method->getName()
            ));
        });

        return $this->resolveArguments($arguments);
    }

    abstract public function getContainer(): ContainerInterface;
}
