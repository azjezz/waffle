<?hh // strict

namespace Waffle\Contract\Http\Server;

use Waffle\Contract\Http\Message\ResponseInterface;
use Waffle\Contract\Http\Message\ServerRequestInterface;

/**
 * An HTTP middleware component participates in processing an HTTP message,
 * either by acting on the request or the response. This interface defines the
 * methods required to use the middleware.
 */
interface MiddlewareInterface
{
    /**
     * Process an incoming server request and return a response, optionally delegating
     * response creation to a handler.
     */
    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface;
}
