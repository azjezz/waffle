<?hh // strict

namespace Waffle\Container\ServiceProvider;

use type IteratorAggregate;
use type Waffle\Container\ContainerAwareInterface;

interface ServiceProviderAggregateInterface extends ContainerAwareInterface, IteratorAggregate<ServiceProviderInterface>
{
    /**
     * Add a service provider to the aggregate.
     *
     * @param string|\Waffle\Container\ServiceProvider\ServiceProviderInterface $provider
     */
    public function add(mixed $provider): this;

    /**
     * Determines whether a service is provided by the aggregate.
     */
    public function provides(string $service): bool;

    /**
     * Invokes the register method of a provider that provides a specific service.
     */
    public function register(string $service): void;
}
