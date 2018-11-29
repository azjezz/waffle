<?hh // strict

namespace Waffle\Container\Definition;

use type Waffle\Container\Argument\ArgumentResolverInterface;
use type Waffle\Container\Argument\ArgumentResolverTrait;
use type Waffle\Container\Argument\ClassNameArgumentInterface;
use type Waffle\Container\Argument\RawArgumentInterface;
use type Waffle\Container\ContainerAwareTrait;
use type ReflectionClass;
use function is_null;
use function is_callable;
use function class_exists;

type MethodDefinition = Pair<
    string,
    Vector<mixed>
>;

class Definition implements ArgumentResolverInterface, DefinitionInterface
{
    use ArgumentResolverTrait;
    use ContainerAwareTrait;

    protected string $alias;

    protected mixed $concrete;

    protected bool $shared = false;

    protected Set<string> $tags;

    protected Vector<mixed> $arguments;

    protected Vector<MethodDefinition> $methods;

    protected mixed $resolved;

    public function __construct(string $id, mixed $concrete = null)
    {
        $concrete = $concrete ?? $id;
        $this->alias    = $id;
        $this->concrete = $concrete;
        $this->tags = Set {};
        $this->arguments = Vector {};
        $this->methods = Vector {};
    }

    public function addTag(string $tag): DefinitionInterface
    {
        $this->tags->add($tag);

        return $this;
    }

    public function hasTag(string $tag): bool
    {
        return $this->tags->contains($tag);
    }

    public function setAlias(string $id): DefinitionInterface
    {
        $this->alias = $id;

        return $this;
    }

    public function getAlias(): string
    {
        return $this->alias;
    }

    public function setShared(bool $shared = true): DefinitionInterface
    {
        $this->shared = $shared;

        return $this;
    }

    public function isShared(): bool
    {
        return $this->shared;
    }

    public function getConcrete(): mixed
    {
        return $this->concrete;
    }

    public function setConcrete(mixed $concrete): DefinitionInterface
    {
        $this->concrete = $concrete;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addArgument(mixed $arg): DefinitionInterface
    {
        $this->arguments->add($arg);

        return $this;
    }

    public function addArguments(Vector<mixed> $args): DefinitionInterface
    {
        $this->arguments->addAll($args);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addMethodCall(string $method, Vector<mixed> $args): DefinitionInterface
    {
        $this->methods->add(Pair {
            $method,
            $args
        });

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addMethodCalls(Map<string, Vector<mixed>> $methods = Map {}): DefinitionInterface
    {
        foreach ($methods as $method => $args) {
            $this->addMethodCall($method, $args);
        }

        return $this;
    }

    public function resolve(bool $new = false): mixed
    {
        $concrete = $this->concrete;

        if ($this->isShared() && ! is_null($this->resolved) && $new === false) {
            return $this->resolved;
        }

        if (is_callable($concrete, false)) {
            /* HH_IGNORE_ERROR[4110]
             * there's no way to tell the type-checker that $concrete is a function */
            $concrete = $this->resolveCallable($concrete);
        }

        if ($concrete instanceof RawArgumentInterface) {
            $this->resolved = $concrete->getValue();

            return $concrete->getValue();
        }

        if ($concrete instanceof ClassNameArgumentInterface) {
            $concrete = $concrete->getValue();
        }

        if (($concrete is string) && class_exists($concrete)) {
            $concrete = $this->resolveClass($concrete);
        }

        if (is_object($concrete)) {
            $concrete = $this->invokeMethods($concrete);
        }

        $this->resolved = $concrete;

        return $concrete;
    }

    protected function resolveCallable((function(mixed ...): mixed) $concrete): mixed
    {
        $resolved = $this->resolveArguments($this->arguments);
        return $concrete(...$resolved);
    }

    protected function resolveClass(string $concrete): mixed
    {
        $resolved   = $this->resolveArguments($this->arguments);
        $reflection = new ReflectionClass($concrete);

        return $reflection->newInstanceArgs($resolved);
    }

    protected function invokeMethods(mixed $instance): mixed
    {
        foreach ($this->methods as $method) {
            $args = $this->resolveArguments(
                $method->lastValue()
            );

            /* HH_IGNORE_ERROR[2025]
             * there's no way to tell the type-checker that $instance is an object */
            $callable = inst_meth($instance, $method->at(0));

            $callable(...$args);
        }

        return $instance;
    }
}
