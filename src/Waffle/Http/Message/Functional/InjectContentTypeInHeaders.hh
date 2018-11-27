<?hh // strict

namespace Waffle\Http\Message\Functional;

use Waffle\Contract\Http\Message\StreamInterface;
use Waffle\Http\Message\Stream;

use function strtolower;
use function array_reduce;

/**
* Inject the provided Content-Type, if none is already present.
*/
function InjectContentTypeInHeaders(string $contentType, Map<string, Set<string>> $headers): Map<string, Set<string>>
{
    $hasContentType = array_reduce(
        $headers->keys()->toArray(),
        ($carry, $item) ==> $carry ?: (strtolower($item) === 'content-type'),
        false
    );

    if (false === $hasContentType) {
        $headers->set('content-type', Set {
            $contentType
        });
    }

    return $headers;
}