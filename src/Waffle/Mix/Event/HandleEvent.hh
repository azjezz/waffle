<?hh // strict

namespace Waffle\Mix\Event;

use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;

class HandleEvent implements EventInterface
{
    public function __construct(
        public ServerRequestInterface $request
    ) {}
}
