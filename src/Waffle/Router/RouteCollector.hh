<?hh // strict

namespace Waffle\Router;

use namespace HH\Lib\Str;
use namespace HH\Lib\Vec;
use namespace HH\Lib\C;
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
    use RouteCollectorTrait;

    public function __construct(
        protected RouterInterface $router
    ) {}
}
