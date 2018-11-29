<?hh

namespace Waffle\Tests\Container;

use Waffle\Container\Definition\DefinitionInterface;
use function Facebook\FBExpect\expect;
use Waffle\Container\Exception\NotFoundException;
use Waffle\Tests\Container\Asset\{Foo, Bar, FooServiceProvider};
use Waffle\Container\{Container, ReflectionContainer};
use Facebook\HackTest\HackTest;

class ContainerTest extends HackTest
{
    /**
     * Asserts that the container can add and get a service.
     */
    public function testContainerAddsAndGets()
    {
        $container = new Container();

        $container->add(Foo::class);

        expect($container->has(Foo::class))->toBeTrue();

        $foo = $container->get(Foo::class);

        expect($foo)->toBeInstanceOf(Foo::class);
    }

    /**
     * Asserts that the container can add and get a service defined as shared.
     */
    public function testContainerAddsAndGetsShared()
    {
        $container = new Container();

        $container->share(Foo::class);

        expect($container->has(Foo::class))->toBeTrue();

        $fooOne = $container->get(Foo::class);
        $fooTwo = $container->get(Foo::class);

        expect($fooOne)->toBeInstanceOf(Foo::class);
        expect($fooTwo)->toBeInstanceOf(Foo::class);
        expect($fooTwo)->toBeSame($fooOne);
    }

    /**
     * Asserts that the container can add and get a service defined as shared.
     */
    public function testContainerAddsAndGetsSharedByDefault()
    {
        $container = (new Container())->defaultToShared();

        $container->add(Foo::class);

        expect($container->has(Foo::class))->toBeTrue();

        $fooOne = $container->get(Foo::class);
        $fooTwo = $container->get(Foo::class);

        expect($fooOne)->toBeInstanceOf(Foo::class);
        expect($fooTwo)->toBeInstanceOf(Foo::class);
        expect($fooTwo)->toBeSame($fooOne);
    }

    /**
     * Asserts that the container can add and get a service defined as non-shared with defaultToShared enabled.
     */
    public function testContainerAddsNonSharedWithSharedByDefault()
    {
        $container = (new Container())->defaultToShared();

        $container->add(Foo::class, null, false);

        expect($container->has(Foo::class))->toBeTrue();

        $fooOne = $container->get(Foo::class);
        $fooTwo = $container->get(Foo::class);

        expect($fooOne)->toBeInstanceOf(Foo::class);
        expect($fooTwo)->toBeInstanceOf(Foo::class);
        expect($fooTwo)->toNotBeSame($fooOne);
    }

    /**
     * Asserts that the container can add and get services by tag.
     */
    public function testContainerAddsAndGetsFromTag()
    {
        $container = new Container();

        $container->add(Foo::class)->addTag('foobar');
        $container->add(Bar::class)->addTag('foobar');

        expect($container->has(Foo::class))->toBeTrue();

        expect($container->has('foobar'))->toBeTrue();

        /** @var Vector $vec */
        $vec = $container->get('foobar');

        expect($vec)->toBeInstanceOf(Vector::class);
        /* HH_IGNORE_ERROR[4064] */
        expect($vec->count())->toBePHPEqual(2);
        /* HH_IGNORE_ERROR[4063] */
        expect($vec[0])->toBeInstanceOf(Foo::class);
        /* HH_IGNORE_ERROR[4063] */
        expect($vec[1])->toBeInstanceOf(Bar::class);
    }

    /**
     * Asserts that the container can add and get a service from service provider.
     */
    public function testContainerAddsAndGetsWithServiceProvider()
    {
        $provider = new FooServiceProvider();

        $container = new Container();

        $container->addServiceProvider($provider);

        //expect($container->has(Foo::class))->toBeTrue();

        $foo = $container->get(Foo::class);

        expect($foo)->toBeInstanceOf(Foo::class);
    }

    /**
     * Asserts that the container can add and get a service from a delegate.
     */
    public function testContainerAddsAndGetsFromDelegate()
    {
        $delegate  = new ReflectionContainer();
        $container = new Container();

        $container->delegate($delegate);

        $foo = $container->get(Foo::class);

        expect($foo)->toBeInstanceOf(Foo::class);
    }

    /**
     * Asserts that the container throws an exception when cannot find service.
     */
    public function testContainerThrowsWhenCannotGetService()
    {
        expect(() ==> {
            $container = new Container();
            expect($container->has(Foo::class))->toBeFalse();
            $container->get(Foo::class);
        })->toThrow(NotFoundException::class);
    }

    /**
     * Asserts that the container can find a definition to extend.
     */
    public function testContainerCanExtendDefinition()
    {
        $container = new Container();

        $container->add(Foo::class);

        $definition = $container->extend(Foo::class);

        expect($definition)->toBeInstanceOf(DefinitionInterface::class);
    }

    /**
     * Asserts that the container can find a definition to extend from service provider.
     */
    public function testContainerCanExtendDefinitionFromServiceProvider()
    {
        $provider = new FooServiceProvider();

        $container = new Container();

        $container->addServiceProvider($provider);

        $definition = $container->extend(Foo::class);

        expect($definition)->toBeInstanceOf(DefinitionInterface::class);
    }

    /**
     * Asserts that the container throws an exception when can't find definition to extend.
     */
    public function testContainerThrowsWhenCannotGetDefinitionToExtend()
    {
        expect(() ==> {
            $container = new Container();
            expect($container->has(Foo::class))->toBeFalse();
            $container->extend(Foo::class);
        })->toThrow(NotFoundException::class);
    }

    /**
     * Asserts that the container adds and invokes an inflector.
     */
    public function testContainerAddsAndInvokesInflector()
    {
        $container = new Container();

        $container->inflector(Foo::class)->setProperty('bar', Bar::class);

        $container->add(Foo::class);
        $container->add(Bar::class);

        $foo = $container->get(Foo::class);

        expect($foo)->toBeInstanceOf(Foo::class);
        /* HH_IGNORE_ERROR[4064] */
        expect($foo->bar)->toBeInstanceOf(Bar::class);
    }
}
