<?hh // strict

namespace Waffle\Http\Server\Middleware;

use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Http\Server\RequestHandler\CallableRequestHandlerDecorator;

final class PathMiddlewareDecorator implements MiddlewareInterface
{
    private string $prefix;

    public function __construct(string $prefix, private MiddlewareInterface $middleware)
    {
        $this->prefix = $this->normalizePrefix($prefix);
    }
    
    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        $path = $request->getUri()->getPath();
        $path = $path === '' ? '/' : $path;
    
        // Current path is shorter than decorator path
        if (\strlen($path) < \strlen($this->prefix)) {
            return $handler->handle($request);
        }
    
        // Current path does not match decorator path
        if (0 !== \stripos($path, $this->prefix)) {
            return $handler->handle($request);
        }

        // Skip if match is not at a border ('/' or end)
        $border = $this->getBorder($path);
        if ($border && '/' !== $border) {
            return $handler->handle($request);
        }

        // Trim off the part of the url that matches the prefix if it is not / only
        $requestToProcess = $this->prefix !== '/'
            ? $this->prepareRequestWithTruncatedPrefix($request)
            : $request;
        
        // Process our middleware.
        // If the middleware calls on the handler, the handler should be provided
        // the original request, as this indicates we've left the path-segregated
        // layer.
        return $this->middleware->process(
            $requestToProcess,
            $this->prepareHandlerForOriginalRequest($handler)
        );
    }

    private function getBorder(string $path): string
    {
        if ($this->prefix === '/') {
            return '/';
        }

        $length = \strlen($this->prefix);

        return \strlen($path) > $length ? $path[$length] : '';
    }

    private function prepareRequestWithTruncatedPrefix(ServerRequestInterface $request): ServerRequestInterface
    {
        $uri  = $request->getUri();
        $path = $this->getTruncatedPath($this->prefix, $uri->getPath());
        $new  = $uri->withPath($path);

        return $request->withUri($new);
    }

    private function getTruncatedPath(string $segment, string $path): string
    {
        if ($segment === $path) {
            // Decorated path and current path are the same; return empty string
            return '';
        }

        // Strip decorated path from start of current path
        return \substr($path, \strlen($segment));
    }

    private function prepareHandlerForOriginalRequest(RequestHandlerInterface $handler): RequestHandlerInterface
    {
        $callable = (ServerRequestInterface $request): ResponseInterface ==> {
            $uri = $request->getUri();
            $uri = $uri->withPath($this->prefix . $uri->getPath());
            return $handler->handle($request->withUri($uri));
        };

        return new CallableRequestHandlerDecorator($callable);
    }

    /**
     * Ensures that the right-most slash is trimmed for prefixes of more than
     * one character, and that the prefix begins with a slash.
     */
    private function normalizePrefix(string $prefix): string
    {
        $prefix = \strlen($prefix) > 1 ? \rtrim($prefix, '/') : $prefix;

        if (0 !== \strpos($prefix, '/')) {
            $prefix = '/' . $prefix;
        }

        return $prefix;
    }
}