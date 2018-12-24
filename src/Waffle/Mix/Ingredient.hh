<?hh // strict

namespace Waffle\Mix;

use namespace Waffle;
use type Waffle\Mix\Application;
use type Waffle\Router\Route;
use type Waffle\Config\Configuration;
use type Waffle\Container\Container;
use type Waffle\Container\Inflector\InflectorInterface;
use type Waffle\Container\Definition\DefinitionInterface;
use type Waffle\Contract\Container\ContainerInterface;
use type Waffle\Contract\Event\EventSubscriberInterface;
use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Event\EventListener;
use type Waffle\Container\ServiceProvider\ServiceProviderInterface;

abstract class Ingredient
{
    final public function __construct(
        private Application $application,
        private Container $container,
    ) {}

    abstract public function mix(): void;

    final protected function config(): Configuration
    {
        return $this->container->get(Configuration::class) as Configuration;
    }

    final protected function on(classname<EventInterface> $event, EventListener $listener, int $priority = 0): void
    {
        $this->application->on($event, $listener, $priority);
    }

    /**
     * Add a service provider.
     */
    final protected function register(ServiceProviderInterface $service): void
    {
        $this->application->register($service);
    }

    /**
     * Register an event subscriber with the dispatcher.
     */
    final protected function subscribe(EventSubscriberInterface $subscriber): void
    {
        $this->application->subscribe($subscriber);
    }

    /*
     * Pipe middleware like unix pipes.
     */
    final protected function pipe(mixed $middleware, int $priority = 0): void
    {
        $this->application->pipe($middleware, $priority);
    }

    final protected function route(string $path, mixed $middleware, ?vec<string> $methods, ?string $name = null): Route
    {
        return $this->application->route($path, $middleware, $methods, $name);
    }

    final protected function get(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['GET'], $name);
    }

    final protected function post(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['POST'], $name);
    }

    final protected function put(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['PUT'], $name);
    }

    final protected function patch(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['PATCH'], $name);
    }

    final protected function delete(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['DELETE'], $name);
    }

    final protected function any(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, null, $name);
    }

    /**
     * Allows for manipulation of specific types on resolution.
     */
    final protected function inflector(string $type, ?(function(mixed): void) $callback = null): InflectorInterface
    {
        return $this->container->inflector($type, $callback);
    }

    /**
     * Get a definition to extend.
     */
    final protected function extend(string $id): DefinitionInterface
    {
        return $this->container->extend($id);
    }

    /**
     * Add an item to the container.
     */
    final protected function add(string $id, mixed $concrete = null, ?bool $shared = null): DefinitionInterface
    {
        return $this->container->add($id, $concrete, $shared);
    }

    /**
     * Proxy to add with shared as true.
     */
    final protected function share(string $id, mixed $concrete = null): DefinitionInterface
    {
        return $this->add($id, $concrete, true);
    }

    final protected function container(): ContainerInterface
    {
        return $this->container;
    }
}
