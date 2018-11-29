<?hh // strict

namespace Waffle\Container\Inflector;

use type Waffle\Container\ContainerAwareTrait;
use type Waffle\Container\Argument\ArgumentResolverInterface;
use type Waffle\Container\Argument\ArgumentResolverTrait;
use type ReflectionProperty;
use function get_class;
use function call_user_func_array;
use function array_combine;

class Inflector implements ArgumentResolverInterface, InflectorInterface
{
    use ArgumentResolverTrait;
    use ContainerAwareTrait;

    protected string $type;

    protected mixed $callback;

    protected Map<string, Vector<mixed>> $methods;

    protected Map<string, mixed> $properties;

    public function __construct(string $type, mixed $callback = null)
    {
        $this->type     = $type;
        $this->callback = $callback;
        $this->properties = Map {};
        $this->methods = Map {};
    }

    /**
     * {@inheritdoc}
     */
    public function getType(): string
    {
        return $this->type;
    }

    /**
     * {@inheritdoc}
     */
    public function invokeMethod(string $name, Vector<mixed> $args): InflectorInterface
    {
        $this->methods->set($name, $args);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function invokeMethods(array<string, Vector<mixed>> $methods): InflectorInterface
    {
        foreach ($methods as $name => $args) {
            $this->invokeMethod($name, $args);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setProperty(string $property, mixed $value): InflectorInterface
    {
        $this->properties->set(
            $property,
            $this->resolveArguments(
                Vector { $value }
            )->firstValue()
        );

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setProperties(array<string, mixed> $properties): InflectorInterface
    {
        foreach ($properties as $property => $value) {
            $this->setProperty($property, $value);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function inflect(mixed $object): void
    {
        $properties = $this->resolveArguments(
            $this->properties->toVector()
        );

        $properties = new Map<string, mixed>(
            array_combine(
                $this->properties->keys(),
                $properties
            )
        );

        foreach ($properties as $property => $value) {
            $reflection = new ReflectionProperty(get_class($object), $property);
            $reflection->setValue($object, $value);
        }

        foreach ($this->methods as $method => $args) {
            $args = $this->resolveArguments($args);

            call_user_func_array([$object, $method], $args);
        }

        if (null !== $this->callback) {
            call_user_func_array($this->callback, [$object]);
        }
    }
}
