<?hh

namespace Waffle\Tests\Container\Asset;

use type Waffle\Container\ServiceProvider\AbstractServiceProvider;

class FooServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        Foo::class
    ];

    <<__Override>>
    public function register(): void
    {
        $this->add(Foo::class);
    }
}
