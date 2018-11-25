<?hh // strict

namespace Waffle\Container\Inflector;

interface InflectorInterface
{
    /**
     * Get the type.
     *
     * @return string
     */
    public function getType(): string;

    /**
     * Defines a method to be invoked on the subject object.
     */
    public function invokeMethod(string $name, Vector<mixed> $args): InflectorInterface;

    /**
     * Defines multiple methods to be invoked on the subject object.
     */
    public function invokeMethods(array<string, Vector<mixed>> $methods): InflectorInterface;

    /**
     * Defines a property to be set on the subject object.
     */
    public function setProperty(string $property, mixed $value): InflectorInterface;

    /**
     * Defines multiple properties to be set on the subject object.
     */
    public function setProperties(array<string, mixed> $properties): InflectorInterface;

    /**
     * Apply inflections to an object.
     */
    public function inflect(mixed $object): void;
}