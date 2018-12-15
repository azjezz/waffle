<?hh // strict

namespace Waffle\Router\Middleware;

use type Waffle\Http\Message\StatusCode;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ResponseFactoryInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Router\RouteResult;

/**
 * Emit a 405 Method Not Allowed response
 *
 * If the request composes a route result, and the route result represents a
 * failure due to request method, this middleware will emit a 405 response,
 * along with an Allow header indicating allowed methods, as reported by the
 * route result.
 *
 * If no route result is composed, and/or it's not the result of a method
 * failure, it passes handling to the provided handler.
 */
class MethodNotAllowedMiddleware implements MiddlewareInterface
{
    public function __construct(
        private ResponseFactoryInterface $responseFactory
    ) {}

    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        $routeResult = $request->getAttribute(RouteResult::class);

        if (! $routeResult is RouteResult || ! $routeResult->isMethodFailure()) {
            return $handler->handle($request);
        }

        return $this->responseFactory->createResponse()
            ->withStatus(StatusCode::STATUS_METHOD_NOT_ALLOWED)
            ->withHeader('Allow', $routeResult->getAllowedMethods() ?? vec[]);
    }
}
