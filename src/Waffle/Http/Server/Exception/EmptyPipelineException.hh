<?hh // strict

namespace Waffle\Http\Server\Exception;

use type Waffle\Contract\Http\Server\MiddlewarePipeInterface;
use type OutOfBoundsException;

<<__ConsistentConstruct>>
class EmptyPipelineException extends OutOfBoundsException implements ExceptionInterface
{
    public static function forClass(classname<MiddlewarePipeInterface> $class): this
    {
        return new static(\sprintf(
            '%s cannot handle request; no middleware available to process the request',
            $class
        ));
    }
}
