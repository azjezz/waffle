<?hh // strict

namespace Waffle\Http\Message\Functional;

use Waffle\Contract\Http\Message\StreamInterface;
use Waffle\Http\Message\Stream;

use function fopen;

/**
 * Create a stream from a string.
 */
function CreateStreamFromString(string $str): StreamInterface
{
    $handle = fopen('php://temp', 'wb+');
    $stream = new Stream($handle);
    $stream->write($str);
    $stream->rewind();
    return $stream;
}