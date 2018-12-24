<?hh // strict

namespace Waffle\Mix\Event;

use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;

class PipeEvent implements EventInterface
{
    public function __construct(
        public MiddlewareInterface $middleware,
        public int $priority
    ) {}
}
