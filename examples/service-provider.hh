<?hh // strict

namespace Example\Container\ServiceProvider;

use namespace Waffle;

require __DIR__.'/../vendor/hh_autoload.hh';

<<__EntryPoint>>
function main(): void
{
    $container = new Waffle\Container\Container();
    $container->delegate(new Waffle\Container\ReflectionContainer());
    $container->addServiceProvider(new Waffle\Http\HttpServiceProvider());
    $container->addServiceProvider(new Waffle\Router\RouterServiceProvider());
    
    assert(
        $container->get(Waffle\Contract\Http\Kernel\KernelInterface::class)
        is Waffle\Contract\Http\Kernel\KernelInterface
    );

    assert(
        $container->get(Waffle\Router\Router::class)
        is Waffle\Router\Router
    );
}