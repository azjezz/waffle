<?hh // strict

namespace Waffle\Http\Message\Exception;

use RuntimeException;

<<__ConsistentConstruct>>
class UntellableStreamException extends RuntimeException implements ExceptionInterface
{
    public static function dueToMissingResource(): this
    {
        return new static('No resource available; cannot tell position');
    }

    public static function dueToPhpError(): this
    {
        return new static('Error occurred during tell operation');
    }
}