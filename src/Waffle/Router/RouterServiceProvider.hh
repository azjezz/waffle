<?hh // strict

namespace Waffle\Router;

use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use type Waffle\Contract\Http\Message\StreamFactoryInterface;
use type Waffle\Contract\Http\Message\ResponseFactoryInterface;

class RouterServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        RouteCollector::class,
        RouterInterface::class,
        Middleware\DispatchMiddleware::class,
        Middleware\ImplicitHeadMiddleware::class,
        Middleware\ImplicitOptionsMiddleware::class,
        Middleware\MethodNotAllowedMiddleware::class,
        Middleware\RouteMiddleware::class,
    ];

    <<__Override>>
    public function register(): void
    {
        $this->share(RouterInterface::class, Router::class);
        $this->share(RouteCollector::class)
            ->addArgument(RouterInterface::class);

        $this->share(Middleware\DispatchMiddleware::class);
        $this->share(Middleware\ImplicitHeadMiddleware::class)
            ->addArgument(RouterInterface::class)
            ->addArgument(StreamFactoryInterface::class);
        $this->share(Middleware\ImplicitOptionsMiddleware::class)
            ->addArgument(ResponseFactoryInterface::class);
        $this->share(Middleware\MethodNotAllowedMiddleware::class)
            ->addArgument(ResponseFactoryInterface::class);
        $this->share(Middleware\RouteMiddleware::class)
            ->addArgument(RouterInterface::class);
    }
}
