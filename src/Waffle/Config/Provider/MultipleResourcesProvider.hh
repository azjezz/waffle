<?hh // strict

namespace Waffle\Config\Provider;

use type Waffle\Lib\Recursive;
use type Generator;

abstract class MultipleResourcesProvider implements ProviderInterface
{
    abstract protected function loadMany(): Generator<string, KeyedContainer<arraykey, mixed>, void>;

    public function load(): KeyedContainer<arraykey, mixed>
    {
        $generator = $this->loadMany();
        $container = dict[];
        foreach ($this->loadMany() as $resource => $config) {
            $container = Recursive::merge($container, $config);
        }
        return $container;
    }
}
