<?hh // strict

namespace Waffle\Config\Provider;

use namespace Waffle\Config\__Private;
use type Generator;

abstract class MultipleResourcesProvider implements ProviderInterface
{
    abstract protected function loadMany(): Generator<string, KeyedContainer<arraykey, mixed>, void>;

    public function load(): KeyedContainer<arraykey, mixed>
    {
        $generator = $this->loadMany();
        $container = dict[];
        foreach ($this->loadMany() as $resource => $config) {
            $container = __Private\merge_recursive($container, $config);
        }
        return $container;
    }
}