<?hh // strict

namespace Waffle\Container\Argument;

use Waffle\Container\Exception\{ContainerException, NotFoundException};
use Waffle\Container\ReflectionContainer;
use Waffle\Contract\Container\ContainerInterface;
use ReflectionFunctionAbstract;
use ReflectionParameter;
use function sprintf;

trait ArgumentResolverTrait
{
    public function resolveArguments(Vector<mixed> $arguments): Vector<mixed>
    {
        $resolved = new Vector<mixed>([]);

        foreach ($arguments as $arg) {
            if ($arg instanceof RawArgumentInterface) {
                $resolved->add($arg->getValue());
                continue;
            }

            if ($arg instanceof ClassNameArgumentInterface) {
                $arg = $arg->getValue();
            }

            if (! is_string($arg)) {
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

    public function reflectArguments(ReflectionFunctionAbstract $method, Map<string, mixed> $args): Vector<mixed>
    {
        $parameters = new Vector<ReflectionParameter>($method->getParameters());

        $arguments = $parameters->map<mixed>(function (ReflectionParameter $param) use ($method, $args) {
            $name  = $param->getName();
            $class = $param->getClass();

            if ($args->contains($name)) {
                return $args->get($name);
            }

            if (null !== $class) {
                return $class->getName();
            }

            if ($param->isDefaultValueAvailable()) {
                return $param->getDefaultValue();
            }

            throw new NotFoundException(sprintf(
                'Unable to resolve a value for parameter (%s) in the function/method (%s)',
                $name,
                $method->getName()
            ));
        });

        return $this->resolveArguments($arguments);
    }

    abstract public function getContainer(): ContainerInterface;
}
