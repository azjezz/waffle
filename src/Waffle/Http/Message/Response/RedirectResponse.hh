<?hh // strict

namespace Waffle\Http\Message\Response;

use namespace Waffle\Http\Message\Functional;
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
     * @param Map<string, Set<string>> $headers Map of headers to use at initialization.
     */
    public function __construct(UriInterface $uri, int $status = 302, Map<string, Set<string>> $headers = Map {})
    {
        $headers->set('location', Set {
            $uri->__toString()
        });

        parent::__construct(
            $status,
            $headers,
            Functional\create_stream_from_string('')
        );
    }
}
