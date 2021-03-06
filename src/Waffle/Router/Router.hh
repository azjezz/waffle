<?hh // strict

namespace Waffle\Router;

use namespace HH\Lib\C;
use namespace HH\Lib\Str;
use namespace HH\Lib\Dict;
use type Facebook\HackRouter\PrefixMatching\PrefixMap;
use type Waffle\Contract\Http\Message\RequestInterface;
use type Facebook\HackRouter\NotFoundException;
use function preg_match;

class Router implements RouterInterface
{
    public function __construct(
        private dict<string, Route> $routes = dict[]
    ) {}

    public function addRoute(Route $route): void
    {
        $this->routes[$route->getName()] = $route;
    }

    public function match(RequestInterface $request): RouteResult
    {
        $method = $request->getMethod();
        $path = $request->getUri()->getPath();
        $routes = $this->marshalMethodRoutes($method);

        if (0 === C\count($routes)) {
            goto failure;
        }

        try {
            $prefixMap = PrefixMap::fromFlatMap($routes);
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

    protected function marshalMethodRoutes(string $method): dict<string, Route>
    {
        $routes = dict[];
        $method = Str\uppercase($method);

        foreach ($this->routes as $name => $route) {
            $allowedMethods = $route->getAllowedMethods();

            if (null === $allowedMethods || C\contains($allowedMethods, $method)) {
                $routes[$route->getPath()] = $route;
            }
        }

        return $routes;
    }

    private function resolveWithMap(string $path, PrefixMap<Route> $map): (Route, dict<string, string>)
    {
        $literals = $map->getLiterals();
        if (C\contains_key($literals, $path)) {
            return tuple($literals[$path], dict[]);
        }

        $prefixes = $map->getPrefixes();
        if (0 !== C\count($prefixes)) {
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

            $data = Dict\filter_keys<string, string>($matches, ($key) ==> $key is string);
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

            return tuple($responder, Dict\merge($data, $sub_data));
        }

        throw new NotFoundException();
    }
}
