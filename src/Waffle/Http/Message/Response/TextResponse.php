<?hh // strict

namespace Waffle\Http\Message\Response;

use Waffle\Http\Message\Response;
use Waffle\Http\Message\Functional;

/**
 * Plain text response.
 *
 * Allows creating a response by passing a string to the constructor;
 * by default, sets a status code of 200 and sets the Content-Type header to
 * text/plain.
 */
class TextResponse extends Response
{
    /**
     * Create a plain text response.
     *
     * Produces a text response with a Content-Type of text/plain and a default
     * status of 200.
     */
    public function __construct(string $text, int $status = 200, Map<string, Set<string>> $headers = Map {})
    {
        parent::__construct(
            $status,
            Functional\InjectContentTypeInHeaders('text/plain; charset=utf-8', $headers),
            Functional\CreateStreamFromString($text),
        );
    }
}
