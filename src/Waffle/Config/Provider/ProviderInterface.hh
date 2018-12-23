<?hh // strict

namespace Waffle\Config\Provider;


interface ProviderInterface
{
    public function load(): KeyedContainer<arraykey, mixed>;
}
