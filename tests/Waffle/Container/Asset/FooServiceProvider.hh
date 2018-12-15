<?hh

namespace Waffle\Tests\Container\Asset;

use Waffle\Container\ServiceProvider\AbstractServiceProvider;

class FooServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        Foo::class
    ];

    public function register(): void
    {
        $this->add(Foo::class);
    }
}
