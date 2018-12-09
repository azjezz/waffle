<?hh // strict

namespace Waffle\Http\Message\__Private;

use type Waffle\Contract\Http\Message\StreamInterface;
use type Waffle\Http\Message\Stream;
use function fopen;

/**
 * Create a stream from a string.
 */
function create_stream_from_string(string $str): StreamInterface
{
    $handle = fopen('php://temp', 'wb+');
    $stream = new Stream($handle);
    $stream->write($str);
    $stream->rewind();
    return $stream;
}
