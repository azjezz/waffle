<?hh // strict

namespace Waffle\Http\Session;

use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;

class SessionMiddleware implements MiddlewareInterface
{
    public function __construct(
        private Persistence\SessionPersistenceInterface $persistence
    ) {}

    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        $session = $this->persistence->initialize($request);

        return $handler->handle($request->withAttribute('session', $session))
            |> $this->persistence->persist($session, $$);
    }
}
