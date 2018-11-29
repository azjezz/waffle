<?hh 

namespace Waffle\Tests\Container\Asset;

use type Waffle\Container\ServiceProvider\{
    AbstractServiceProvider, 
    BootableServiceProviderInterface
};

class FakeServiceProvider extends AbstractServiceProvider implements BootableServiceProviderInterface
{
    protected ?Set<string> $provides = Set {
        'SomeService',
        'AnotherService'
    };

    public int $registered = 0;

    public function boot(): void
    {
        // noop
    }

    public function register(): void
    {
        $this->registered++;

        $this->add('SomeService', function (mixed $arg): mixed {
            return $arg;
        });
    }

}
