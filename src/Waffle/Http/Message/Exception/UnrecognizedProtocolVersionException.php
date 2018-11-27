<?hh // strict

namespace Waffle\Http\Message\Exception;

use namespace HH\Lib\Str;
use UnexpectedValueException;

<<__ConsistentConstruct>>
class UnrecognizedProtocolVersionException extends UnexpectedValueException implements ExceptionInterface
{
    public static function forVersion(string $version): this
    {
        return new static(
            Str\format('Unrecognized protocol version (%s)', $version)
        );
    }
}