<?hh // strict

namespace Waffle\Config\Provider;

class ContainerProvider implements ProviderInterface
{
    public function __construct(
        private KeyedContainer<arraykey, mixed> $container
    ) {}

    public function load(): KeyedContainer<arraykey, mixed>
    {
        return $this->container;
    }
}
