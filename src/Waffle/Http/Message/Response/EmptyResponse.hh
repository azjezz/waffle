<?hh // strict

namespace Waffle\Http\Message\Response;

use Waffle\Http\Message\Response;
use Waffle\Http\Message\Stream;

use function fopen;

/**
 * A class representing empty HTTP responses.
 */
class EmptyResponse extends Response
{
    /**
     * Create an empty response with the given status code.
     *
     * @param int $status Status code for the response, if any.
     * @param Map<string, Set<string>> $headers Headers for the response, if any.
     */
    public function __construct(int $status = 204, Map<string, Set<string>> $headers = Map {})
    {
        $body = new Stream(fopen('php://temp', 'r'));
        parent::__construct($status, $headers, $body);
    }

    /**
     * Create an empty response with the given headers.
     *
     * @param Map<string, Set<string>> $headers Headers for the response.
     * @return EmptyResponse
     */
    public static function withHeaders(Map<string, Set<string>> $headers): EmptyResponse
    {
        return new self(204, $headers);
    }
}
