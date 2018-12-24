<?hh // strict

namespace Waffle\Mix\Event;

use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

class EmitEvent implements EventInterface
{
    public function __construct(
        public ResponseInterface $response
    ) {}
}
