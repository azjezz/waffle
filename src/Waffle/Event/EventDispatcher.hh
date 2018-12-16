<?hh // strict

namespace Waffle\Event;

use type Waffle\Contract\Event\EventSubscriberInterface;
use type Waffle\Contract\Event\EventDispatcherInterface;
use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Event\StoppableEventInterface;
use type Waffle\Contract\Event\EventListener;
use type Waffle\Contract\Service\ResetInterface;

use type SplPriorityQueue;
use function get_class;

class EventDispatcher implements EventDispatcherInterface, ResetInterface
{
    private dict<
        classname<EventInterface>,
        SplPriorityQueue<EventListener>
    > $listeners = dict[];

    /**
     * Register an event listener with the dispatcher.
     */
    public function on(classname<EventInterface> $event, EventListener $listener, int $priority = 0): void
    {
        $listeners = $this->listeners[$event] ?? new SplPriorityQueue<EventListener>();
        $listeners->insert($listener, $priority);
        $this->listeners[$event] = $listeners;
    }

    /**
     * Register an event subscriber with the dispatcher.
     */
    public function subscribe(EventSubscriberInterface $subscriber): void
    {
        $subscriber->subscribe($this);
    }

    /**
     * Dispatch an event and call the listeners.
     */
    public function dispatch(EventInterface $event): EventInterface
    {
        $name = get_class($event);
        $listeners = $this->listeners[$name] ?? vec[];

        foreach ($listeners as $listener) {
            $listener($event);
            if ($event is StoppableEventInterface && $event->isPropagationStopped()) {
                break;
            }
        }

        return $event;
    }

    /**
     * Remove a set of listeners from the dispatcher.
     */
    public function forget(classname<EventInterface> $event): void
    {
        unset($this->listeners[$event]);
    }

    public function reset(): void
    {
        $this->listeners = dict[];
    }
}
