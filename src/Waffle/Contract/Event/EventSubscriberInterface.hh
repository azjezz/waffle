<?hh // strict

namespace Waffle\Contract\Event;

interface EventSubscriberInterface
{
    /*
     *
     */
    public function subscribe(EventDispatcherInterface $events): void;
}
