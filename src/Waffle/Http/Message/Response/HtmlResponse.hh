<?hh // strict

namespace Waffle\Http\Message\Response;

use namespace Waffle\Http\Message\__Private;
use type Waffle\Http\Message\Response;

/**
 * HTML response.
 *
 * Allows creating a response by passing an HTML string to the constructor;
 * by default, sets a status code of 200 and sets the Content-Type header to
 * text/html.
 */
class HtmlResponse extends Response
{
    /**
     * Create an HTML response.
     *
     * Produces an HTML response with a Content-Type of text/html and a default
     * status of 200.
     */
    public function __construct(string $html, int $status = 200, Map<string, Set<string>> $headers = Map {})
    {
        parent::__construct(
            $status,
            __Private\inject_content_type_in_headers('text/html; charset=utf8', $headers),
            __Private\create_stream_from_string($html),
        );
    }
}
