<?hh // strict

namespace Waffle\Container\Definition;

use namespace HH\Lib\C;
use namespace HH\Lib\Vec;
use type Waffle\Container\Argument\ArgumentResolverInterface;
use type Waffle\Container\Argument\ArgumentResolverTrait;
use type Waffle\Container\Argument\ClassNameArgumentInterface;
use type Waffle\Container\Argument\RawArgumentInterface;
use type Waffle\Container\ContainerAwareTrait;
use type ReflectionClass;
use function is_null;
use function is_callable;
use function class_exists;

class Definition implements ArgumentResolverInterface, DefinitionInterface
{
    use ArgumentResolverTrait;
    use ContainerAwareTrait;

    protected string $alias;

    protected mixed $concrete;

    protected bool $shared = false;

    protected vec<string> $tags = vec[];

    protected vec<mixed> $arguments = vec[];

    protected dict<string, vec<mixed>> $methods = dict[];

    protected mixed $resolved;

    public function __construct(string $id, mixed $concrete = null)
    {
        $concrete = $concrete ?? $id;
        $this->alias    = $id;
        $this->concrete = $concrete;
    }

    public function addTag(string $tag): DefinitionInterface
    {
        $this->tags[] = $tag;

        return $this;
    }

    public function hasTag(string $tag): bool
    {
        return C\contains($this->tags, $tag);
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
        $this->arguments[] = $arg;

        return $this;
    }

    public function addArguments(vec<mixed> $args): DefinitionInterface
    {
        $this->arguments = Vec\concat($this->arguments, $args);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addMethodCall(string $method, vec<mixed> $args): DefinitionInterface
    {
        $this->methods[$method] = $args;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addMethodCalls(dict<string, vec<mixed>> $methods = dict[]): DefinitionInterface
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
        $constructor = $reflection->getConstructor();

        if (null !== $constructor) {
            while(C\count($resolved) < $constructor->getNumberOfRequiredParameters())
            {
                $resolved[] = null;
            }
        }

        return $reflection->newInstanceArgs($resolved);
    }

    protected function invokeMethods(mixed $instance): mixed
    {
        foreach ($this->methods as $method => $args) {
            $args = $this->resolveArguments($args);

            /* HH_IGNORE_ERROR[2025]
             * there's no way to tell the type-checker that $instance is an object */
            $callable = inst_meth($instance, $method);

            $callable(...$args);
        }

        return $instance;
    }
}
