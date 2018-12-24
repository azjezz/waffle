<?hh // strict

namespace Waffle\Mix\Handler;

use namespace HH\Lib\Str;
use namespace Waffle\Http\Message;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

class NotFoundHandler implements RequestHandlerInterface
{
    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        return new Message\Response\JsonResponse([
            'status' => 'error',
            'message' => Str\format('Cannot %s %s', $request->getMethod(), $request->getRequestTarget())
        ], Message\StatusCode::STATUS_NOT_FOUND);
    }
}
