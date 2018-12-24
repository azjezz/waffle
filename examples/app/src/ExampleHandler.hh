<?hh // strict

namespace Example\App;

use namespace Waffle\Http\Message;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

class ExampleHandler implements RequestHandlerInterface
{
    public function handle(ServerRequestInterface $request): ResponseInterface
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

        return new Message\Response\JsonResponse($data, Message\StatusCode::STATUS_OK)
            |> $$->withCookie('name', new Message\Cookie('value'))
            |> $$->withCookie('another-name', new Message\Cookie('another-value'))
            |> $$->withAddedHeader('Tk', vec(['N']));
    }
}
