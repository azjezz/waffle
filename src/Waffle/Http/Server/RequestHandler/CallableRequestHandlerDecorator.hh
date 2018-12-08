<?hh

namespace Waffle\Http\Server\RequestHandler;

use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

final class CallableRequestHandlerDecorator implements RequestHandlerInterface
{
    public function __construct(
       private (function(ServerRequestInterface): ResponseInterface) $callback
    ) {}

    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        $fun = $this->callback;

        return $fun($request);
    }
}
