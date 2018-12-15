<?hh // strict

namespace Waffle\Http\Message\__Private;

use namespace HH\Lib\Str;
use namespace HH\Lib\C;

/**
* Inject the provided Content-Type, if none is already present.
*/
function inject_content_type_in_headers(string $contentType, dict<string, vec<string>> $headers): dict<string, vec<string>>
{
    $hasContentType = C\reduce_with_key(
        $headers,
        ($carry, $key, $item) ==> $carry ?: (Str\lowercase($key) === 'content-type'),
        false
    );

    if (false === $hasContentType) {
        $headers['content-type'] = vec[
            $contentType
        ];
    }

    return $headers;
}
