<?hh // strict

namespace Waffle\Router;

use namespace HH\Lib\Str;
use type Waffle\Contract\Http\Server\MiddlewareInterface;

/**
 * Aggregate routes for the router.
 *
 * This class provides * methods for creating path+HTTP method-based routes and
 * injecting them into the router:
 *
 * - get
 * - post
 * - put
 * - patch
 * - delete
 * - any
 *
 * A general `route()` method allows specifying multiple request methods and/or
 * arbitrary request methods when creating a path-based route.
 *
 * Internally, the class performs some checks for duplicate routes when
 * attaching via one of the exposed methods, and will raise an exception when a
 * collision occurs.
 */
class RouteCollector
{
    protected RouterInterface $router;

    /**
     * List of all routes registered directly with the application.
     */
    private Vector<Route> $routes = Vector {};

    public function __construct(RouterInterface $router)
    {
        $this->router = $router;
    }

    /**
     * Add a route for the route middleware to match.
     *
     * Accepts a combination of a path and middleware, and optionally the HTTP methods allowed.
     *
     * @param null|Set<string> $methods HTTP method to accept; null indicates any.
     * @param null|string $name The name of the route.
     * @throws Exception\DuplicateRouteException if specification represents an existing route.
     */
    public function route(
        string $path,
        MiddlewareInterface $middleware,
        ?Set<string> $methods = null,
        ?string $name = null
    ) : Route {
        $this->checkForDuplicateRoute($path, $methods);

        $route   = new Route($path, $middleware, $methods, $name);
        $this->routes->add($route);
        $this->router->addRoute($route);
        return $route;
    }

    /**
     * @param null|string $name The name of the route.
     */
    public function get(string $path, MiddlewareInterface $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, Set { 'GET' }, $name);
    }

    /**
     * @param null|string $name The name of the route.
     */
    public function post(string $path, MiddlewareInterface $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, Set { 'POST' }, $name);
    }

    /**
     * @param null|string $name The name of the route.
     */
    public function put(string $path, MiddlewareInterface $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, Set { 'PUT' }, $name);
    }

    /**
     * @param null|string $name The name of the route.
     */
    public function patch(string $path, MiddlewareInterface $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, Set { 'PATCH' }, $name);
    }

    /**
     * @param null|string $name The name of the route.
     */
    public function delete(string $path, MiddlewareInterface $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, Set { 'DELETE' }, $name);
    }

    /**
     * @param null|string $name The name of the route.
     */
    public function any(string $path, MiddlewareInterface $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, null, $name);
    }

    /**
     * Retrieve all directly registered routes with the application.
     */
    public function getRoutes(): Vector<Route>
    {
        return $this->routes;
    }

    /**
     * Determine if the route is duplicated in the current list.
     *
     * Checks if a route with the same name or path exists already in the list;
     * if so, and it responds to any of the $methods indicated, raises
     * a DuplicateRouteException indicating a duplicate route.
     *
     * @throws Exception\DuplicateRouteException on duplicate route detection.
     */
    private function checkForDuplicateRoute(string $path, ?Set<string> $methods = null): void
    {
        $matches = $this->routes->filter((Route $route) ==> {
            if ($path !== $route->getPath()) {
                return false;
            }

            if ($methods === null) {
                return true;
            }

            $carry = false;
            foreach ($methods as $method) {
                $carry = $carry || $route->allowsMethod($method);
            }

            return $carry;
        });

        $match = $matches->firstValue();

        if (0 !== $matches->count() && $match is nonnull) {
            $allowedMethods = $match->getAllowedMethods() ?? ['(any)'];
            $name = $match->getName();

            throw new Exception\DuplicateRouteException(Str\format(
                'Duplicate route detected; path "%s" answering to methods [%s]%s',
                $match->getPath(),
                Str\join($allowedMethods, ','),
                $name !== null ? Str\format(', with name "%s"', $name) : ''
            ));
        }
    }
}
