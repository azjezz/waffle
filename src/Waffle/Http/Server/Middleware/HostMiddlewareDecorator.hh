<?hh // strict

namespace Waffle\Http\Server\Middleware;

use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;

class HostMiddlewareDecorator implements MiddlewareInterface
{
    public function __construct(
        private string $host,
        private MiddlewareInterface $middleware
    ) {}

    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        $host = $request->getUri()->getHost();

        if ($host !== \strtolower($this->host)) {
            return $handler->handle($request);
        }

        return $this->middleware->process($request, $handler);
    }
}
