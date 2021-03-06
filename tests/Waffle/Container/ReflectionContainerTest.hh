<?hh

namespace Waffle\Tests\Container;

use type Waffle\Container\Exception\NotFoundException;
use function Facebook\FBExpect\expect;
use type Waffle\Container\ReflectionContainer;
use type Waffle\Container\Container;
use type Waffle\Tests\Container\Asset\{Foo, FooCallable, Bar};
use type Facebook\HackTest\HackTest;

class ReflectionContainerTest extends HackTest
{
    /**
     * Asserts that ReflectionContainer claims it has an item if a class exists for the alias.
     */
    public function testHasReturnsTrueIfClassExists()
    {
        $container = new ReflectionContainer();

        expect($container->has(ReflectionContainer::class))->toBeTrue();
    }

    /**
     * Asserts that ReflectionContainer denies it has an item if a class does not exist for the alias.
     */
    public function testHasReturnsFalseIfClassDoesNotExist()
    {
        $container = new ReflectionContainer();

        expect($container->has('blah'))->toBeFalse();
    }

    /**
     * Asserts that ReflectionContainer instantiates a class that does not have a constructor.
     */
    public function testContainerInstantiatesClassWithoutConstructor()
    {
        $classWithoutConstructor = \stdClass::class;

        $container = new ReflectionContainer();

        expect($container->get($classWithoutConstructor))->toBeInstanceOf($classWithoutConstructor);
    }

    /**
     * Asserts that ReflectionContainer instantiates and cacheds a class that does not have a constructor.
     */
    public function testContainerInstantiatesAndCachesClassWithoutConstructor()
    {
        $classWithoutConstructor = \stdClass::class;

        $container = (new ReflectionContainer())->cacheResolutions();

        $classWithoutConstructorOne = $container->get($classWithoutConstructor);
        $classWithoutConstructorTwo = $container->get($classWithoutConstructor);

        expect($classWithoutConstructorOne)->toBeInstanceOf($classWithoutConstructor);
        expect($classWithoutConstructorTwo)->toBeInstanceOf($classWithoutConstructor);
        expect($classWithoutConstructorTwo)->toBeSame($classWithoutConstructorOne);
    }

    /**
     * Asserts that ReflectionContainer instantiates a class that has a constructor.
     */
    public function testGetInstantiatesClassWithConstructor()
    {
        $classWithConstructor = Foo::class;
        $dependencyClass      = Bar::class;

        $container = new ReflectionContainer();

        $container->setContainer($container);

        $item = $container->get($classWithConstructor);

        expect($item)->toBeInstanceOf($classWithConstructor);
        /* HH_IGNORE_ERROR[4064] */
        expect($item->bar)->toBeInstanceOf($dependencyClass);
    }

    /**
     * Asserts that ReflectionContainer instantiates and caches a class that has a constructor.
     */
    public function testGetInstantiatesAndCachedClassWithConstructor()
    {
        $classWithConstructor = Foo::class;
        $dependencyClass      = Bar::class;

        $container = (new ReflectionContainer())->cacheResolutions();

        $container->setContainer($container);

        $itemOne = $container->get($classWithConstructor);
        $itemTwo = $container->get($classWithConstructor);

        expect($itemOne)->toBeInstanceOf($classWithConstructor);
        /* HH_IGNORE_ERROR[4064] */
        expect($itemOne->bar)->toBeInstanceOf($dependencyClass);

        expect($itemTwo)->toBeInstanceOf($classWithConstructor);
        /* HH_IGNORE_ERROR[4064] */
        expect($itemTwo->bar)->toBeInstanceOf($dependencyClass);

        expect($itemTwo)->toBeSame($itemOne);
        /* HH_IGNORE_ERROR[4064] */
        expect($itemTwo->bar)->toBeSame($itemOne->bar);
    }

    /**
     * Asserts that ReflectionContainer instantiates a class that has a constructor with a type-hinted argument, and
     * fetches that dependency from the container injected into the ReflectionContainer.
     */
    public function testGetInstantiatesClassWithConstructorAndUsesContainer()
    {
        $classWithConstructor = Foo::class;
        $dependencyClass      = Bar::class;

        $dependency = new Bar();
        $container  = new ReflectionContainer();

        $innerContainer = new Container();
        $innerContainer->add($dependencyClass, () ==> $dependency);

        $container->setContainer($innerContainer);

        $item = $container->get($classWithConstructor);

        expect($item)->toBeInstanceOf($classWithConstructor);
        /* HH_IGNORE_ERROR[4064] */
        expect($item->bar)->toBeSame($dependency);
    }

    /**
     * Asserts that ReflectionContainer instantiates a class that has a constructor with a type-hinted argument, and
     * uses the values provided in the argument array.
     */
    public function testGetInstantiatesClassWithConstructorAndUsesArguments()
    {
        $classWithConstructor = Foo::class;
        $dependencyClass      = Bar::class;

        $dependency = new Bar();
        $container  = new ReflectionContainer();

        $innerContainer = new Container();
        $innerContainer->add($dependencyClass, () ==> $dependency);

        $container->setContainer($innerContainer);

        $item = $container->get($classWithConstructor);

        expect($item)->toBeInstanceOf($classWithConstructor);
        /* HH_IGNORE_ERROR[4064] */
        expect($item->bar)->toBeSame($dependency);
    }

    /**
     * Asserts that an exception is thrown when attempting to get a class that does not exist.
     */
    public function testThrowsWhenGettingNonExistentClass()
    {
        expect(() ==> {
            $container = new ReflectionContainer();
            $container->get('Whoooooopyyyy');
        })->toThrow(NotFoundException::class);
    }

    /**
     * Asserts that call reflects on a closure and injects arguments.
     */
    public function testCallReflectsOnClosureArguments()
    {
        $container = new ReflectionContainer();

        $foo = $container->call(function (Foo $foo) {
            return $foo;
        });

        expect($foo)->toBeInstanceOf(Foo::class);
        /* HH_IGNORE_ERROR[4064] */
        expect($foo->bar)->toBeInstanceOf(Bar::class);
    }

    /**
     * Asserts that call reflects on an instance method and injects arguments.
     */
    public function testCallReflectsOnInstanceMethodArguments()
    {
        $container = new ReflectionContainer();
        $foo       = new Foo(null);

        $container->call([$foo, 'setBar']);

        expect($foo)->toBeInstanceOf(Foo::class);
        expect($foo->bar)->toBeInstanceOf(Bar::class);
    }

    /**
     * Asserts that call reflects on a static method and injects arguments.
     */
    public function testCallReflectsOnStaticMethodArguments()
    {
        $container = new ReflectionContainer();

        $container->setContainer($container);

        $container->call('Waffle\Tests\Container\Asset\Foo::staticSetBar');

        expect(Asset\Foo::$staticBar)->toBeInstanceOf(Bar::class);
        expect(Asset\Foo::$staticHello)->toBePHPEqual('hello world');
    }

    /**
     * Asserts that exception is thrown when an argument cannot be resolved.
     */
    public function testThrowsWhenArgumentCannotBeResolved()
    {
        expect(() ==> {
            $container = new ReflectionContainer();
            $container->call([new Bar(), 'setSomething']);
        })->toThrow(NotFoundException::class);
    }

    /**
     * Tests the support for __invokable/callable classes for the ReflectionContainer::call method.
     */
    public function testInvokableClass()
    {
        $container = new ReflectionContainer();

        $foo = $container->call(new FooCallable(), dict[ 'bar' => new Bar() ]);

        expect($foo)->toBeInstanceOf(Foo::class);
        /* HH_IGNORE_ERROR[4064] */
        expect($foo->bar)->toBeInstanceOf(Bar::class);
    }
}
