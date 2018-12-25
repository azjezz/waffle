<?hh // strict

namespace Example\App\Handler;

use namespace Waffle\Http\Message;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use function time;

class PingHandler implements RequestHandlerInterface
{
    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        $data = dict[
            'status' => 'success',
            'data' => dict[
                'ping' => 'pong',
                'time' => time()
            ]
        ];

        return new Message\Response\JsonResponse($data, Message\StatusCode::STATUS_OK)
            |> $$->withAddedHeader('Tk', vec(['N']));
    }
}