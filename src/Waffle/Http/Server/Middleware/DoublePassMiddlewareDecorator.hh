<?hh // strict

namespace Waffle\Http\Server\Middleware;

use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Http\Message\Response;

class DoublePassMiddlewareDecorator implements MiddlewareInterface
{
    public function __construct(
        private (function(ServerRequestInterface, ResponseInterface, (function(ServerRequestInterface): ResponseInterface)): ResponseInterface) $call,
        private ResponseInterface $response = new Response()
    ) {}

    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        $fun = $this->call;
        return $fun($request, $this->response, (ServerRequestInterface $req): ResponseInterface ==> $handler->handle($req));
    }
}
