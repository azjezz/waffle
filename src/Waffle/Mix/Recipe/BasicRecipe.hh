<?hh // strict

namespace Waffle\Mix\Recipe;

use namespace Waffle\Mix\Middleware;
use namespace Waffle\Router\Middleware as Router;
use namespace Waffle\Mix\Handler;
use type Waffle\Mix\Error\ErrorHandlerInterface;
use type Waffle\Mix\Error\ErrorHandler;
use type Waffle\Mix\Environment;

class BasicRecipe extends Recipe
{
    <<__Override>>
    public function mix(): void
    {
        $this->share(ErrorHandlerInterface::class, ErrorHandler::class)
            ->addArgument(Environment::class);
        $this->share(Middleware\ErrorMiddleware::class)
            ->addArgument(ErrorHandlerInterface::class);
        $this->share(Handler\NotFoundHandler::class);

        /*
         *  The error handler should be the first (most outer) middleware to catch
         *  all Exceptions.
         */
        $this->pipe(Middleware\ErrorMiddleware::class, 0x10000);

        /*
         * Register the routing middleware in the middleware pipeline.
         * This middleware register the Waffle\Router\RouteResult request attribute.
         */
        $this->pipe(Router\RouteMiddleware::class, -0x9500);

        /*
         * The following handle routing failures for common conditions:
         * - HEAD request but no routes answer that method
         * - OPTIONS request but no routes answer that method
         * - method not allowed
         * Order here maters; the
         */
        $this->pipe(Router\ImplicitHeadMiddleware::class, -0x9600);
        $this->pipe(Router\ImplicitOptionsMiddleware::class, -0x9700);
        $this->pipe(Router\MethodNotAllowedMiddleware::class, -0x9800);

        /*
         * Register the dispatch middleware in the middleware pipeline.
         */
        $this->pipe(Router\DispatchMiddleware::class, -0x09900);

        /*
         * At this point, if no Response is returned by any middleware, the
         * NotFoundHandler kicks in; alternately, you can provide other fallback
         * middleware to execute.
         */
        $this->pipe(Handler\NotFoundHandler::class, -0x10000);
    }
}
