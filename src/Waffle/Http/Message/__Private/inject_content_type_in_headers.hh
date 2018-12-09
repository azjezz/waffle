<?hh // strict

namespace Waffle\Http\Message\__Private;

use namespace HH\Lib\Str;
use function array_reduce;

/**
* Inject the provided Content-Type, if none is already present.
*/
function inject_content_type_in_headers(string $contentType, Map<string, Set<string>> $headers): Map<string, Set<string>>
{
    $hasContentType = array_reduce(
        $headers->keys()->toArray(),
        ($carry, $item) ==> $carry ?: (Str\lowercase($item) === 'content-type'),
        false
    );

    if (false === $hasContentType) {
        $headers->set('content-type', Set {
            $contentType
        });
    }

    return $headers;
}
