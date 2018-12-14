<?hh

namespace Waffle\Tests\Container\Definition;

use namespace HH\Lib\Str;
use Waffle\Contract\Container\ContainerInterface;
use Waffle\Container\Argument\ClassNameArgument;
use Waffle\Container\Definition\Definition;
use Waffle\Container\Argument\RawArgument;
use Waffle\Container\Container;
use Waffle\Tests\Container\Asset\Foo;
use Waffle\Tests\Container\Asset\FooCallable;
use Waffle\Tests\Container\Asset\Bar;
use Facebook\HackTest\HackTest;
use function Facebook\FBExpect\expect;

class DefinitionTest extends HackTest
{
    /**
     * Asserts that the definition can resolve a closure with defined args.
     */
    public function testDefinitionResolvesClosureWithDefinedArgs()
    {
        $definition = new Definition('callable', (...$args) ==> Str\join($args, ' '));

        $definition->addArguments(vec[ 'hello', 'world' ]);

        $actual = $definition->resolve();

        expect($actual)->toBeSame('hello world');
    }

    /**
     * Asserts that the definition can resolve a closure returning raw argument.
     */
    public function testDefinitionResolvesClosureReturningRawArgument()
    {
        $definition = new Definition('callable', function () {
            return new RawArgument('hello world');
        });

        $actual = $definition->resolve();

        expect($actual)->toBeSame('hello world');
    }

    /**
     * Asserts that the definition can resolve a callable class.
     */
    public function testDefinitionResolvesCallableClass()
    {
        $definition = new Definition('callable', new FooCallable());

        $definition->addArgument(new Bar());

        $actual = $definition->resolve();

        expect($actual)->toBeInstanceOf(Foo::class);
    }

    /**
     * Asserts that the definition can resolve an array callable.
     */
    public function testDefinitionResolvesArrayCallable()
    {
        $definition = new Definition('callable', [new FooCallable(), '__invoke']);

        $definition->addArgument(new Bar());

        $actual = $definition->resolve();

        expect($actual)->toBeInstanceOf(Foo::class);
    }

    /**
     * Asserts that the definition can resolve a class method calls.
     */
    public function testDefinitionResolvesClassWithMethodCalls()
    {

            $container = new Container();

            $bar = new Bar();

            $container->add(Bar::class, (): Bar ==> $bar);

            $definition = new Definition('callable', Foo::class);

            $definition->setContainer($container);
            $definition->addArgument(null);
            $definition->addMethodCalls(dict[
                'setBar' => vec[ Bar::class ]
            ]);

            $actual = $definition->resolve();

            expect($actual)->toBeInstanceOf(Foo::class);
            /* HH_IGNORE_ERROR[4064] */
            expect($actual->bar)->toBeInstanceOf(Bar::class);

    }

    /**
     * Asserts that the definition can resolve a class with defined args.
     */
    public function testDefinitionResolvesClassWithDefinedArgs()
    {

        $container = new Container();
        $bar = new Bar();
        $container->add(Bar::class, (): Bar ==> $bar);

        $definition = new Definition('callable', Foo::class);

        $definition->setContainer($container);
        $definition->addArgument(Bar::class);

        $actual = $definition->resolve();

        expect($actual)->toBeInstanceOf(Foo::class);
        /* HH_IGNORE_ERROR[4064] */
        expect($actual->bar)->toBeInstanceOf(Bar::class);
    }

    /**
     * Asserts that the definition can resolve a class as class name.
     */
    public function testDefinitionResolvesClassAsClassNameArgument()
    {
        $container = new Container();
        $bar = new Bar();
        $container->add(Bar::class, (): Bar ==> $bar);

        $definition = new Definition('callable', new ClassNameArgument(Foo::class));

        $definition->setContainer($container);
        $definition->addArgument(new ClassNameArgument(Bar::class));

        $actual = $definition->resolve();

        expect($actual)->toBeInstanceOf(Foo::class);
        /* HH_IGNORE_ERROR[4064] */
        expect($actual->bar)->toBeInstanceOf(Bar::class);
    }

    /**
     * Asserts that the definition resolves a shared item only once.
     */
    public function testDefinitionResolvesSharedItemOnlyOnce()
    {
        $definition = new Definition('callable', new ClassNameArgument(Bar::class));

        $definition->setShared(true);

        $actual1 = $definition->resolve();
        $actual2 = $definition->resolve();
        $actual3 = $definition->resolve(true);

        expect($actual2)->toBeSame($actual1);
        expect($actual1)->toNotBeSame($actual3);
    }

    /**
     * Asserts that the definition can add tags.
     */
    public function testDefinitionCanAddTags()
    {
        $definition = new Definition('callable', new ClassNameArgument(Foo::class));

        $definition
            ->addTag('tag1')
            ->addTag('tag2');

        expect($definition->hasTag('tag1'))->toBeTrue();
        expect($definition->hasTag('tag2'))->toBeTrue();
        expect($definition->hasTag('tag3'))->toBeFalse();
    }

    /**
     * Assert that the definition returns the concrete.
     */
    public function testDefinitionCanGetConcrete()
    {
        $concrete = new ClassNameArgument(Foo::class);
        $definition = new Definition('callable', $concrete);

        expect($definition->getConcrete())->toBeSame($concrete);
    }

    /**
     * Assert that the definition set the concrete.
     */
    public function testDefinitionCanSetConcrete()
    {
        $definition = new Definition('callable', null);

        $concrete = new ClassNameArgument(Foo::class);
        $definition->setConcrete($concrete);

        expect($definition->getConcrete())->toBeSame($concrete);
    }
}
