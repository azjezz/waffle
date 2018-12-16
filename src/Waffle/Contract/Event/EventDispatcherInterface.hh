<?hh // strict

namespace Waffle\Contract\Event;

interface EventDispatcherInterface
{
    /**
     * Register an event listener with the dispatcher.
     */
    public function on<TEvent super EventInterface>(classname<TEvent> $event, EventListener<TEvent> $listener, int $priority = 0): void;

    /**
     * Register an event subscriber with the dispatcher.
     */
    public function subscribe(EventSubscriberInterface $subscriber): void;

    /**
     * Dispatch an event and call the listeners.
     */
    public function dispatch<TEvent super EventInterface>(TEvent $event, bool $halt = false): TEvent;

    /**
     * Remove a set of listeners from the dispatcher.
     */
    public function forget<TEvent super EventInterface>(classname<TEvent> $event): void;
}
