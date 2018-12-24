<?hh // strict

namespace Waffle\Mix\Event;

use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Event\EventSubscriberInterface;

class SubscribeEvent implements EventInterface
{
    public function __construct(
        public EventSubscriberInterface $subscriber
    ) {}
}
