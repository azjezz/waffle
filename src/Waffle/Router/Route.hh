<?hh // strict

namespace Waffle\Router;

use namespace HH\Lib\Str;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use function preg_match;

/**
 * Value object representing a single route.
 *
 * Routes are a combination of path, middleware, and HTTP methods; two routes
 * representing the same path and overlapping HTTP methods are not allowed,
 * while two routes representing the same path and non-overlapping HTTP methods
 * can be used (and should typically resolve to different middleware).
 *
 * Internally, only those three properties are required. However, underlying
 * router implementations may allow or require additional information, such as
 * information defining how to generate a URL from the given route, qualifiers
 * for how segments of a route match, or even default values to use. These may
 * be provided after instantiation via the "options" property and related
 * setOptions() method.
 */
class Route implements MiddlewareInterface
{
    const HTTP_METHOD_SEPARATOR = ':';

    private dict<string, mixed> $options = dict[];

    private ?Set<string> $methods = null;

    private string $name;

    /**
     * @param string $path Path to match.
     * @param MiddlewareInterface $middleware Middleware to use when this route is matched.
     * @param null|string[] $methods Allowed HTTP methods; defaults to HTTP_METHOD_ANY.
     * @param null|string $name the route name
     */
    public function __construct(
        private string $path,
        private MiddlewareInterface $middleware,
        ?Set<string> $methods = null,
        ?string $name = null
    ) {
        if (null !== $methods) {
            $this->methods = $this->validateHttpMethods($methods);
        }

        if (null === $name) {
            $name = $this->methods === null
                ? $path
                : $path . '^' . Str\join($this->methods, self::HTTP_METHOD_SEPARATOR);
        }

        $this->name = $name;
    }

    /**
     * Proxies to the middleware composed during instantiation.
     */
    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        return $this->middleware->process($request, $handler);
    }

    public function getPath(): string
    {
        return $this->path;
    }

    /**
     * Set the route name.
     */
    public function setName(string $name): void
    {
        $this->name = $name;
    }

    public function getName(): string
    {
        return $this->name;
    }

    public function getMiddleware(): MiddlewareInterface
    {
        return $this->middleware;
    }

    /**
     * @return null|Set<string> Returns null or set of allowed methods.
     */
    public function getAllowedMethods(): ?Set<string>
    {
        return $this->methods;
    }

    /**
     * Indicate whether the specified method is allowed by the route.
     *
     * @param string $method HTTP method to test.
     */
    public function allowsMethod(string $method): bool
    {
        $method = Str\uppercase($method);

        if (null === $this->methods || $this->methods->contains($method)) {
            return true;
        }

        return false;
    }

    public function setOptions(dict<string, mixed> $options): void
    {
        $this->options = $options;
    }

    public function getOptions(): dict<string, mixed>
    {
        return $this->options;
    }

    /**
     * Validate the provided HTTP method names.
     *
     * Validates, and then normalizes to upper case.
     *
     * @param Set<string> A Set of HTTP method names.
     * @return string[]
     * @throws Exception\InvalidArgumentException for any invalid method names.
     */
    private function validateHttpMethods(Set<string> $methods): Set<string>
    {
        if (0 === $methods->count()) {
            throw new Exception\InvalidArgumentException(
                'HTTP methods argument was empty; must contain at least one method'
            );
        }

        $valid = true;
        foreach ($methods as $method) {
            if (! preg_match('/^[!#$%&\'*+.^_`\|~0-9a-z-]+$/i', $method)) {
                $valid = false;
            }
        }

        if (!$valid) {
           throw new Exception\InvalidArgumentException('One or more HTTP methods were invalid');
        }

        return $methods->map(($m) ==> Str\uppercase($m));
    }
}
