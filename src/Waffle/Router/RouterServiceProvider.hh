<?hh // strict

namespace Waffle\Router;

use type Waffle\Container\ServiceProvider\AbstractServiceProvider;

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
        $this->share(RouteCollector::class);
        $this->share(RouterInterface::class, Router::class);

        $this->share(Middleware\DispatchMiddleware::class);
        $this->share(Middleware\ImplicitHeadMiddleware::class);
        $this->share(Middleware\ImplicitOptionsMiddleware::class);
        $this->share(Middleware\MethodNotAllowedMiddleware::class);
        $this->share(Middleware\RouteMiddleware::class);
    }
}
