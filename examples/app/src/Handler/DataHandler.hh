<?hh // strict

namespace Example\App\Handler;


use namespace Waffle\Http\Message;
use type Waffle\Contract\Http\Session\SessionInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

class DataHandler implements RequestHandlerInterface
{
    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        $session = $request->getAttribute('session') as SessionInterface;
        $session->set('i', (int) $session->get('i', 1) + 1);
        $data = $request->getAttribute('data') as dict<_, _>;
        return new Message\Response\JsonResponse($data, Message\StatusCode::STATUS_OK)
            |>$$->withCookie('name', new Message\Cookie('value'))
                ->withCookie('another-name', new Message\Cookie('another-value'))
                ->withAddedHeader('Tk', vec(['N']));
    }
}
