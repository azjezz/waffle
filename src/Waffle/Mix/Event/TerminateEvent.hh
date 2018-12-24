<?hh // strict

namespace Waffle\Mix\Event;

use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

class TerminateEvent implements EventInterface
{
    public function __construct(
        public ServerRequestInterface $request,
        public ResponseInterface $response
    ) {}
}
