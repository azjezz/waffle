<?hh // strict

namespace Waffle\Http\Message\Response;

use namespace Waffle\Http\Message\Functional;
use type Waffle\Http\Message\Response;

/**
 * XML response.
 *
 * Allows creating a response by passing an XML string to the constructor; by default,
 * sets a status code of 200 and sets the Content-Type header to application/xml.
 */
class XmlResponse extends Response
{
    /**
     * Create an XML response.
     *
     * Produces an XML response with a Content-Type of application/xml and a default
     * status of 200.
     */
    public function __construct(string $xml, int $status = 200, Map<string, Set<string>> $headers = Map {})
    {
        parent::__construct(
            $status,
            Functional\inject_content_type_in_headers('application/xml; charset=utf8', $headers),
            Functional\create_stream_from_string($xml),
        );
    }
}
