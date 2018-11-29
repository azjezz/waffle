<?hh // strict

namespace Waffle\Container\Definition;

use type Waffle\Container\ContainerAwareInterface;

interface DefinitionInterface extends ContainerAwareInterface
{
    /**
     * Add a tag to the definition.
     */
    public function addTag(string $tag): DefinitionInterface;

    /**
     * Does the definition have a tag?
     */
    public function hasTag(string $tag): bool;

    /**
     * Set the alias of the definition.
     */
    public function setAlias(string $id): DefinitionInterface;

    /**
     * Get the alias of the definition.
     */
    public function getAlias(): string;

    /**
     * Set whether this is a shared definition.
     */
    public function setShared(bool $shared): DefinitionInterface;

    /**
     * Is this a shared definition?
     */
    public function isShared(): bool;

    /**
     * Get the concrete of the definition.
     */
    public function getConcrete(): mixed;

    /**
     * Set the concrete of the definition.
     */
    public function setConcrete(mixed $concrete): DefinitionInterface;

    /**
     * Add an argument to be injected.
     */
    public function addArgument(mixed $arg): DefinitionInterface;

    /**
     * Add multiple arguments to be injected.
     */
    public function addArguments(Vector<mixed> $args): DefinitionInterface;

    /**
     * Add a method to be invoked
     */
    public function addMethodCall(string $method, Vector<mixed> $args): DefinitionInterface;

    /**
     * Add multiple methods to be invoked
     */
    public function addMethodCalls(Map<string, Vector<mixed>> $methods = Map {}): DefinitionInterface;

    /**
     * Handle instantiation and manipulation of value and return.
     *
     * @param boolean $new
     *
     * @return mixed
     */
    public function resolve(bool $new = false): mixed;
}
