<?hh // strict

namespace Example\App;

use namespace Waffle\Http\Message\Response;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

class LocalHandler implements RequestHandlerInterface
{
    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        return new Response\JsonResponse(dict[
            'status' => 'success',
            'data' => [
                'request' => $request->getAttributes(),
            ]
        ]);
    }
}
