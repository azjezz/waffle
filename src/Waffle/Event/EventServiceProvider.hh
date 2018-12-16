<?hh // strict

namespace Waffle\Event;

use type Waffle\Contract\Event\EventDispatcherInterface;
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;

class EventServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        EventDispatcherInterface::class,
    ];

    <<__Override>>
    public function register(): void
    {
        $this->share(
            EventDispatcherInterface::class, 
            () ==> new EventDispatcher()
        );
    }
}
