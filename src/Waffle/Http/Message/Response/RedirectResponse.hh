<?hh // strict

namespace Waffle\Http\Message\Response;

use Waffle\Contract\Http\Message\UriInterface;
use Waffle\Http\Message\Response;
use Waffle\Http\Message\Functional;

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
            Functional\CreateStreamFromString('')
        );
    }
}
