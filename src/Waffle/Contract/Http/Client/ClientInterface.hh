<?hh // strict

namespace Waffle\Contract\Http\Client;

use type Waffle\Contract\Http\Message\RequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

interface ClientInterface
{
    /**
     * Sends a request and returns a response.
     *
     * @param RequestInterface $request
     *
     * @return ResponseInterface
     *
     * @throws \Waffle\Contract\Http\Client\ClientExceptionInterface If an error happens while processing the request.
     */
    public function send(RequestInterface $request): ResponseInterface;
}
