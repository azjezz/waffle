<?hh // strict

namespace Waffle\Contract\Event;

interface EventSubscriberInterface
{
    /*
     * Subscribe to the event dispatcher.
     */
    public function subscribe(EventDispatcherInterface $events): void;
}
