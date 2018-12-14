<?hh // strict

namespace Waffle\Http\Message\Response;

use namespace Waffle\Http\Message\__Private;
use type Waffle\Contract\Http\Message\UriInterface;
use type Waffle\Http\Message\Response;

/**
 * Produce a redirect response.
 */
class RedirectResponse extends Response
{
    /**
     * Create a redirect response.
     *
     * Produces a redirect response with a Location header and the given status
     * (302 by default).
     *
     * Note: this method overwrites the `location` $headers value.
     *
     * @param int $status Integer status code for the redirect; 302 by default.
     * @param dict<string, vec<string>> $headers Map of headers to use at initialization.
     */
    public function __construct(UriInterface $uri, int $status = 302, dict<string, vec<string>> $headers = dict[])
    {
        $headers['location'] = vec [
            $uri as string
        ];

        parent::__construct(
            $status,
            $headers,
            __Private\create_stream_from_string('')
        );
    }
}
