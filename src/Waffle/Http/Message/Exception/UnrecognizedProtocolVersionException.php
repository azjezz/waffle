<?hh // strict

namespace Waffle\Http\Message\Exception;

use UnexpectedValueException;

use function sprintf;

<<__ConsistentConstruct>>
class UnrecognizedProtocolVersionException extends UnexpectedValueException implements ExceptionInterface
{
    public static function forVersion(string $version): this
    {
        return new static(
            sprintf('Unrecognized protocol version (%s)', $version)
        );
    }
}