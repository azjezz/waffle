<?hh // strict

namespace Waffle\Mix\Event;

use type Waffle\Contract\Event\EventInterface;
use type Waffle\Container\ServiceProvider\ServiceProviderInterface;

class RegisterEvent implements EventInterface
{
    public function __construct(
        public ServiceProviderInterface $service
    ) {}
}
