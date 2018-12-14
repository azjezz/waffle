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
    public function pipe(MiddlewareInterface $middleware): void
    {
        $this->pipe->pipe($middleware);
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
     *
     * Implementations of this method may be written in such a way as to have
     * side effects, such as usage of header() or pushing output to the
     * output buffer.
     *
     * Implementations MAY raise exceptions if they are unable to emit the
     * response; e.g., if headers have already been sent.
     *
     * Implementations MUST return a boolean. A boolean `true` indicates that
     * the emitter was able to emit the response, while `false` indicates
     * it was not.
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
}
