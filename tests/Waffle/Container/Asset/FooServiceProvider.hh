<?hh

namespace Waffle\Tests\Container\Asset;

use Waffle\Container\ServiceProvider\AbstractServiceProvider;

class FooServiceProvider extends AbstractServiceProvider
{
    protected ?Set<string> $provides = Set {
        Foo::class
    };

    public function register(): void
    {
        $this->add(Foo::class);
    }
}
