<?hh // strict

namespace Waffle\Http\Message\__Private;

use namespace HH\Lib\Str;
use namespace HH\Lib\C;

/**
* Inject the provided Content-Type, if none is already present.
*/
function inject_content_type_in_headers(string $contentType, Map<string, Set<string>> $headers): Map<string, Set<string>>
{
    $hasContentType = C\reduce(
        $headers->keys(),
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
