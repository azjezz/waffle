<?hh // strict

namespace Example\App\Middleware;

use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;

class DataMiddleware implements MiddlewareInterface
{
    /**
     * Process an incoming server request and return a response, optionally delegating
     * response creation to a handler.
     */
    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        $data = dict([
            'status' => 'success',
            'data' => dict([
                'protocol' => $request->getProtocolVersion(),
                'method' => $request->getMethod(),
                'uri' => dict([
                    'scheme' => $request->getUri()->getScheme(),
                    'user-info' => $request->getUri()->getUserInfo(),
                    'host' => $request->getUri()->getHost(),
                    'port' => $request->getUri()->getPort(),
                    'path' => $request->getUri()->getPath(),
                    'query' => $request->getUri()->getQuery(),
                    'fragment' => $request->getUri()->getFragment(),
                    'authority' => $request->getUri()->getAuthority(),
                ]),
                'query-paramters' => $request->getQueryParams(),
                'headers' => $request->getHeaders(),
                'server-parameters' => $request->getServerParams(),
                'cookies' => $request->getCookieParams(),
                'body' => dict([
                    'parsed' => $request->getParsedBody(),
                    'stream' => $request->getBody()->getContents()
                ]),
                'attributes' => $request->getAttributes(),
            ])
        ]);

        return $handler->handle(
            $request->withAttribute('data', $data)
        );
    }
}
