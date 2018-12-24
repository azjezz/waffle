<?hh // strict

namespace Waffle\Mix\Middleware;

use type Waffle\Mix\Error\ErrorHandlerInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Throwable;

class ErrorMiddleware implements MiddlewareInterface
{
    public function __construct(
        private ErrorHandlerInterface $handler
    ) {}

    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        try {
            return $handler->handle($request);
        } catch(Throwable $e) {
            return $this->handler->handle($e, $request);
        }
    }
}
