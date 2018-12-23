<?hh // strict

namespace Waffle\Config\Provider;

use type Generator;

interface ProviderInterface
{
    public function load(): KeyedContainer<arraykey, mixed>;
}
