<?hh // strict

namespace Waffle\Contract\Http\Server;

interface MiddlewarePipeInterface extends MiddlewareInterface, RequestHandlerInterface
{
    public function pipe(MiddlewareInterface $middleware): void;
}
