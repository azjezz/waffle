<?hh

namespace Waffle\Tests\Container\Asset;

class FooCallable
{
    public function __invoke(Bar $bar)
    {
        return new Foo($bar);
    }
}
