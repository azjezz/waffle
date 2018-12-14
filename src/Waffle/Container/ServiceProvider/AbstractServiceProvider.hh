<?hh // strict

namespace Waffle\Container\ServiceProvider;

use namespace HH\Lib\C;
use type Waffle\Container\ContainerAwareTrait;
use type Waffle\Container\Container;
use type Waffle\Container\Exception\ContainerException;
use type Waffle\Container\Inflector\InflectorInterface;
use type Waffle\Container\Definition\DefinitionInterface;
use function get_class;

abstract class AbstractServiceProvider implements ServiceProviderInterface
{
    use ContainerAwareTrait;

    protected vec<string> $provides = vec[];

    protected string $identifier;

    public function __construct()
    {
        $this->identifier = get_class($this);
    }

    /**
     * {@inheritdoc}
     */
    public function provides(string $alias): bool
    {
        return C\contains($this->provides, $alias);
    }

    /**
     * {@inheritdoc}
     */
    public function setIdentifier(string $id): ServiceProviderInterface
    {
        $this->identifier = $id;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getIdentifier(): string
    {
        return $this->identifier;
    }

    public function getWaffleContainer(): Container
    {
        $container = $this->getContainer();

        if ($container is Container) {
            return $container;
        }

        throw new ContainerException('The current container is not an instance of Waffel\Container\Container');
    }

    /**
     * Allows for manipulation of specific types on resolution.
     */
    public function inflector(string $type, ?(function(mixed): void) $callback = null): InflectorInterface
    {
        return $this->getWaffleContainer()->inflector($type, $callback);
    }

    /**
     * Get a definition to extend.
     */
    public function extend(string $id): DefinitionInterface
    {
        return $this->getWaffleContainer()->extend($id);
    }

    /**
     * Add an item to the container.
     */
    public function add(string $id, mixed $concrete = null, ?bool $shared = null): DefinitionInterface
    {
        return $this->getWaffleContainer()->add($id, $concrete, $shared);
    }

    /**
     * Proxy to add with shared as true.
     */
    public function share(string $id, mixed $concrete = null): DefinitionInterface
    {
        return $this->add($id, $concrete, true);
    }
}
