<?hh // strict

namespace Waffle\Container;

use Waffle\Container\Definition\DefinitionAggregate;
use Waffle\Container\Definition\DefinitionInterface;
use Waffle\Container\Definition\DefinitionAggregateInterface;
use Waffle\Container\Exception\NotFoundException;
use Waffle\Container\Inflector\InflectorAggregate;
use Waffle\Container\Inflector\InflectorInterface;
use Waffle\Container\Inflector\InflectorAggregateInterface;
use Waffle\Container\ServiceProvider\ServiceProviderAggregate;
use Waffle\Container\ServiceProvider\ServiceProviderAggregateInterface;
use Waffle\Contract\Container\ContainerInterface;
use function sprintf;

class Container implements ContainerInterface
{
    protected bool $defaultToShared = false;

    public function __construct(
        protected DefinitionAggregateInterface      $definitions = new DefinitionAggregate(),
        protected ServiceProviderAggregateInterface $providers = new ServiceProviderAggregate(),
        protected InflectorAggregateInterface       $inflectors = new InflectorAggregate(),
        protected Vector<ContainerInterface>        $delegates = Vector {}
    ) {
        if ($this->definitions instanceof ContainerAwareInterface) {
            $this->definitions->setContainer($this);
        }

        if ($this->providers instanceof ContainerAwareInterface) {
            $this->providers->setContainer($this);
        }

        if ($this->inflectors instanceof ContainerAwareInterface) {
            $this->inflectors->setContainer($this);
        }
    }

    /**
     * Add an item to the container.
     */
    public function add(string $id, mixed $concrete = null, ?bool $shared = null): DefinitionInterface
    {
        $concrete = $concrete ?? $id;
        $shared = $shared ?? $this->defaultToShared;
        return $this->definitions->add($id, $concrete, $shared);
    }

    /**
     * Proxy to add with shared as true.
     */
    public function share(string $id, mixed $concrete = null): DefinitionInterface
    {
        return $this->add($id, $concrete, true);
    }

    /**
     * Whether the container should default to defining shared definitions.
     */
    public function defaultToShared(bool $shared = true): this
    {
        $this->defaultToShared = $shared;

        return $this;
    }

    /**
     * Get a definition to extend.
     */
    public function extend(string $id): DefinitionInterface
    {
        if ($this->providers->provides($id)) {
            $this->providers->register($id);
        }

        if ($this->definitions->has($id)) {
            return $this->definitions->getDefinition($id);
        }

        throw new NotFoundException(
            sprintf('Unable to extend alias (%s) as it is not being managed as a definition', $id)
        );
    }

    /**
     * Add a service provider.
     *
     * @param ServiceProviderInterface|string $provider
     */
    public function addServiceProvider(mixed $provider): this
    {
        $this->providers->add($provider);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $id, bool $new = false): mixed
    {
        if ($this->definitions->has($id)) {
            
            $resolved = $this->definitions->resolve($id, $new);

            return $this->inflectors->inflect($resolved);
        }

        if ($this->definitions->hasTag($id)) {
            $set = $this->definitions->resolveTagged($id);

            return $set->map(
                ($resolved) ==> $this->inflectors->inflect($resolved)
            );
        }

        if ($this->providers->provides($id)) {
            $this->providers->register($id);
            return $this->get($id, $new);
        }

        foreach ($this->delegates as $delegate) {
            if ($delegate->has($id)) {
                $resolved = $delegate->get($id);
                return $this->inflectors->inflect($resolved);
            }
        }

        throw new NotFoundException(sprintf('Alias (%s) is not being managed by the container or delegates', $id));
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $id): bool
    {
        if ($this->definitions->has($id)) {
            return true;
        }

        if ($this->definitions->hasTag($id)) {
            return true;
        }

        if ($this->providers->provides($id)) {
            return true;
        }

        foreach ($this->delegates as $delegate) {
            if ($delegate->has($id)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Allows for manipulation of specific types on resolution.
     */
    public function inflector(string $type, mixed $callback = null): InflectorInterface
    {
        return $this->inflectors->add($type, $callback);
    }

    /**
     * Delegate a backup container to be checked for services if it
     * cannot be resolved via this container.
     */
    public function delegate(ContainerInterface $container): this
    {
        $this->delegates[] = $container;

        if ($container instanceof ContainerAwareInterface) {
            $container->setContainer($this);
        }

        return $this;
    }
}
