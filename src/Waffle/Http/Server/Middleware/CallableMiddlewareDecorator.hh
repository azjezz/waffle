<?hh // strict

namespace Waffle\Http\Server\Middleware;

use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;

/**
 * Decorate callable middleware as PSR-15 middleware.
 *
 * Decorates middleware with the following signature:
 *
 * <code>
 * function (
 *     ServerRequestInterface $request,
 *     RequestHandlerInterface $handler
 * ): ResponseInterface
 * </code>
 *
 * such that it will operate as PSR-15 middleware.
 *
 * Neither the arguments nor the return value need be typehinted; however, if
 * the signature is incompatible, a PHP Error will likely be thrown.
 */
final class CallableMiddlewareDecorator implements MiddlewareInterface
{
    public function __construct(
        private (function(ServerRequestInterface, RequestHandlerInterface): ResponseInterface) $middleware)
    {}

    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        $fun = $this->middleware;

        return $fun($request, $handler);
    }
}
