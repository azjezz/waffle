<?hh // strict

namespace Waffle\Router;

use namespace HH\Lib\C;
use namespace HH\Lib\Str;
use type Facebook\HackRouter\PrefixMatching\PrefixMap;
use type Waffle\Contract\Http\Message\RequestInterface;
use type Facebook\HackRouter\NotFoundException;
use function preg_match;
use function array_filter;
use function array_merge;
use const ARRAY_FILTER_USE_KEY;

class Router implements RouterInterface
{
    public function __construct(
        private Map<string, Route> $routes = Map {}
    ) {}

    public function addRoute(Route $route): void
    {
        $this->routes->set($route->getName(), $route);
    }

    public function match(RequestInterface $request): RouteResult
    {
        $method = $request->getMethod();
        $path = $request->getUri()->getPath();
        $routes = $this->marshalMethodRoutes($method);

        if (0 === $routes->count()) {
            goto failure;
        }

        try {
            $prefixMap = $this->createPrefixMap($routes);
            list($route, $params) = $this->resolveWithMap($path, $prefixMap);
            return RouteResult::fromRoute($route, $params);

        } catch(NotFoundException $e) {
            goto failure;
        }

        failure:
        $allowedMethods = Set {};

        foreach ($this->routes as $route) {
            if ($path !== $route->getPath()) {
                continue;
            }

            $allowedMethods->addAll($route->getAllowedMethods());
        }

        return RouteResult::fromRouteFailure(
            $allowedMethods->count() > 0 ?
            $allowedMethods :
            null
        );
    }

    protected function marshalMethodRoutes(string $method): Map<string, Route>
    {
        $routes = Map {};
        $method = Str\uppercase($method);

        foreach ($this->routes as $name => $route) {
            $allowedMethods = $route->getAllowedMethods();

            if (null === $allowedMethods || $allowedMethods->contains($method)) {
                $routes->set($name, $route);
            }
        }

        return $routes;
    }

    private function resolveWithMap(string $path, PrefixMap<Route> $map): (Route, array<string, string>)
    {
        $literals = $map->getLiterals();
        if (C\contains_key($literals, $path)) {
            return tuple($literals[$path], []);
        }

        $prefixes = $map->getPrefixes();
        if (!C\is_empty($prefixes)) {
            $prefix_len = Str\length(C\first_keyx($prefixes));
            $prefix = Str\slice($path, 0, $prefix_len);

            if (C\contains_key($prefixes, $prefix)) {
                return $this->resolveWithMap(
                    Str\strip_prefix($path, $prefix),
                    $prefixes[$prefix],
                );
            }
        }

        $regexps = $map->getRegexps();

        foreach ($regexps as $regexp => $sub_map) {
            $pattern = '#^'.$regexp.'#';
            $matches = [];

            if (preg_match($pattern, $path, &$matches) !== 1) {
                continue;
            }

            $matched = $matches[0];
            $remaining = Str\strip_prefix($path, $matched);

            /* HH_IGNORE_ERROR[4105] builtins_array.hhi contains wrong signature for array_filter */
            $data = array_filter(
                $matches,
                $key ==> ($key is string),
                /* HH_IGNORE_ERROR[2049] builtins_array.hhi doesn't containe the ARRAY_FILTER_* constants */
                /* HH_IGNORE_ERROR[4106] builtins_array.hhi doesn't containe the ARRAY_FILTER_* constants */
                ARRAY_FILTER_USE_KEY
            );
            $sub = $regexps[$regexp];

            if ($sub->isResponder()) {
                if ($remaining === '') {
                    return tuple($sub->getResponder(), $data);
                }

                continue;
            }

            try {
                list($responder, $sub_data) = $this->resolveWithMap($remaining, $sub->getMap());
            } catch (NotFoundException $_) {
                continue;
            }

            return tuple($responder, array_merge($data, $sub_data));
        }

        throw new NotFoundException();
    }

    private function createPrefixMap(Map<string, Route> $routes): PrefixMap<Route>
    {
        $flatMap = dict[];

        foreach ($routes as $route) {
            $flatMap[$route->getPath()] = $route;
        }

        return PrefixMap::fromFlatMap($flatMap);
    }
}
