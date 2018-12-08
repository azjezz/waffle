<?hh // strict

namespace Waffle\Http\Server\Exception;

use namespace HH\Lib\Str;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type InvalidArgumentException as ParentException;

<<__ConsistentConstruct>>
class InvalidMiddlewareException extends ParentException implements ExceptionInterface
{
    public static function forMiddleware(mixed $middleware): this
    {
        return new static(Str\format(
            'Middleware "%s" is neither a string service name, a "%s" instance, a "%s" instance or a callable with a valid middleware, double-pass middleware or handler signatures',
            \is_object($middleware) ? \get_class($middleware) : \gettype($middleware),
            MiddlewareInterface::class,
            RequestHandlerInterface::class,
        ));
    }
}
