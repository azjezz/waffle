<?hh // strict

namespace Waffle\Http\Server\__Private;

use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type SplQueue;

class NextMiddlewareProcessor implements RequestHandlerInterface
{
    private SplQueue<MiddlewareInterface> $queue;

    public function __construct(SplQueue<MiddlewareInterface> $queue, private RequestHandlerInterface $handler)
    {
        $this->queue = clone $queue;
    }

    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        if (0 === $this->queue->count()) {
            return $this->handler->handle($request);
        }

        $middleware = $this->queue->dequeue();

        return $middleware->process($request, $this);
    }
}
