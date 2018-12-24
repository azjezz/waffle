<?hh // strict

namespace Waffle\Mix\Event;

use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Event\StoppableEventInterface;
use type Throwable;

class ErrorEvent implements EventInterface, StoppableEventInterface
{
    public bool $handled = false;

    public function __construct(
        public Throwable $error,
    ) {}

    public function isPropagationStopped() : bool
    {
        return $this->handled;
    }
}
