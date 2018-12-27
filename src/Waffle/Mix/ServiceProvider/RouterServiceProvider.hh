<?hh // strict

namespace Waffle\Mix\ServiceProvider;

use namespace Waffle\Router;
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use type Waffle\Contract\Http\Message\StreamFactoryInterface;
use type Waffle\Contract\Http\Message\ResponseFactoryInterface;

class RouterServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        Router\RouteCollector::class,
        Router\RouterInterface::class,
        Router\Middleware\DispatchMiddleware::class,
        Router\Middleware\ImplicitHeadMiddleware::class,
        Router\Middleware\ImplicitOptionsMiddleware::class,
        Router\Middleware\MethodNotAllowedMiddleware::class,
        Router\Middleware\RouteMiddleware::class,
    ];

    <<__Override>>
    public function register(): void
    {
        $this->share(Router\RouterInterface::class, Router\Router::class);
        $this->share(Router\RouteCollector::class)
            ->addArgument(Router\RouterInterface::class);

        $this->share(Router\Middleware\DispatchMiddleware::class);
        $this->share(Router\Middleware\ImplicitHeadMiddleware::class)
            ->addArgument(Router\RouterInterface::class)
            ->addArgument(StreamFactoryInterface::class);
        $this->share(Router\Middleware\ImplicitOptionsMiddleware::class)
            ->addArgument(ResponseFactoryInterface::class);
        $this->share(Router\Middleware\MethodNotAllowedMiddleware::class)
            ->addArgument(ResponseFactoryInterface::class);
        $this->share(Router\Middleware\RouteMiddleware::class)
            ->addArgument(Router\RouterInterface::class);
    }
}
