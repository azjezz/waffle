<?hh // strict

namespace Waffle\Http\Kernel;

use type Waffle\Contract\Http\Kernel\KernelInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Emitter\EmitterInterface;
use type Waffle\Contract\Http\Server\MiddlewarePipeInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;

class Kernel implements KernelInterface
{
    public function __construct(
        protected MiddlewarePipeInterface $pipe,
        protected EmitterInterface $emitter,
    ) {}

    /*
     * Pipe middleware like unix pipes.
     */
    public function pipe(MiddlewareInterface $middleware, int $priority = 0): void
    {
        $this->pipe->pipe($middleware, $priority);
    }

    /**
     * Process an incoming server request and return a response, optionally delegating
     * response creation to a handler.
     */
    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        return $this->pipe->process($request, $handler);
    }

    /**
     * Handle the request and return a response.
     */
    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        return $this->pipe->handle($request);
    }

    /**
     * Emit a response.
     *
     * Emits a response, including status line, headers, and the message body,
     * according to the environment.
     */
    public function emit(ResponseInterface $response): bool
    {
        return $this->emitter->emit($response);
    }

    /**
     * Perform any final actions for the request lifecycle.
     */
    public function terminate(ServerRequestInterface $request, ResponseInterface $response): void
    {
        $request->getBody()->close();
        $response->getBody()->close();
    }

    public function reset(): void
    {
        $this->emitter->reset();
        $this->pipe->reset();
    }
}
