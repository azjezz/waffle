<?hh

namespace Waffle\Tests\Container\Definition;

use type Facebook\HackTest\HackTest;
use type Waffle\Container\Container;
use type Waffle\Tests\Container\Asset\Foo;
use type Waffle\Tests\Container\Asset\Bar;
use type Waffle\Container\Definition\Definition;
use type Waffle\Container\Exception\NotFoundException;
use type Waffle\Container\Definition\DefinitionInterface;
use type Waffle\Container\Definition\DefinitionAggregate;
use function Facebook\FBExpect\expect;

class DefinitionAggregateTest extends HackTest
{
    /**
     * Asserts that the aggregate can add a definition.
     */
    public function testAggregateAddsDefinition(): void
    {
        $container = new Container();
        $foo = new Foo(null);
        $definition = new Definition('service', (): Foo ==> $foo);

        $aggregate  = (new DefinitionAggregate())->setContainer($container);
        $definition = $aggregate->add('alias', $definition);

        expect($definition)->toBeInstanceOf(DefinitionInterface::class);
        expect($aggregate->resolve('alias'))->toBeSame($foo);
    }

    /**
     * Asserts that the aggregate can create a definition.
     */
    public function testAggregateCreatesDefinition(): void
    {
        $container  = new Container();
        $aggregate  = (new DefinitionAggregate())->setContainer($container);
        $definition = $aggregate->add('alias', Foo::class);

        expect($definition)->toBeInstanceOf(DefinitionInterface::class);
        expect($definition->getAlias())->toBeSame('alias');
    }

    /**
     * Asserts that the aggregate has a definition.
     */
    public function testAggregateHasDefiniton(): void
    {
        $container  = new Container();
        $aggregate  = (new DefinitionAggregate())->setContainer($container);
        $definition = $aggregate->add('alias', Foo::class);

        expect($definition)->toBeInstanceOf(DefinitionInterface::class);
        expect($aggregate->has('alias'))->toBeTrue();
        expect($aggregate->has('nope'))->toBeFalse();
    }

    /**
     * Asserts that the aggregate adds and iterates multiple definitions.
     */
    public function testAggregateAddsAndIteratesMultipleDefinitions(): void
    {
        $container  = new Container();
        $aggregate = (new DefinitionAggregate())->setContainer($container);

        $definitions = [];

        for ($i = 0; $i < 10; $i++) {
            $definitions[] = $aggregate->add('alias' . $i, Foo::class);
        }

        foreach ((array) $aggregate->getIterator() as $key => $definition) {
            expect($definition)->toBeSame($definitions[$key]);
        }
    }

    /**
     * Asserts that the aggregate iterates and resolves a definition.
     */
    public function testAggregateIteratesAndResolvesDefinition(): void
    {
        $aggregate   = new DefinitionAggregate();
        $container = new Container();

        $bar = new Bar();
        $foo = new Foo($bar);

        $definition1 = $container->add('bar', (): Bar ==> $bar);
        $definition2 = $container->add('foo', (): Foo ==> $foo);

        $aggregate->setContainer($container);

        $aggregate->add('alias1', $definition1);
        $aggregate->add('alias2', $definition2, true);

        $resolved = $aggregate->resolve('alias2');
        expect($resolved)->toBeSame($foo);
        /* HH_IGNORE_ERROR[4064] */
        expect($resolved->bar)->toBeSame($bar);
    }

    /**
     * Asserts that the aggregate can resolved array of tagged definitions.
     */
    public function testAggregateCanResolveArrayOfTaggedDefinitions(): void
    {
        $container = new Container();

        $definition1 = $container->add('bar', (): string ==> 'definition1');
        $definition2 = $container->add('foo', (): string ==> 'definition2');

        $definition1->addTag('tag');
        $definition2->addTag('tag');

        $aggregate = new DefinitionAggregate(vec[
            $definition1, $definition2
        ]);

        $aggregate->setContainer($container);

        expect($aggregate->hasTag('tag'))->toBeTrue();

        $resolved = $aggregate->resolveTagged('tag');

        expect($resolved)->toBeSame(vec[
            'definition1', 'definition2'
        ]);
    }

    /**
     * Asserts that the aggregate throws an exception when a definition cannot be resolved.
     */
    public function testAggregateThrowsExceptionWhenCannotResolve(): void
    {
        expect(() ==> {
            $container = new Container();

            $definition1 = $container->add('bar', (): string ==> 'definition1');
            $definition2 = $container->add('foo', (): string ==> 'definition2');

            $definition1->addTag('tag');
            $definition2->addTag('tag');

            $aggregate = new DefinitionAggregate();

            $aggregate->setContainer($container);
            $aggregate->add('alias1', $definition1);
            $aggregate->add('alias2', $definition2, true);
            $resolved = $aggregate->resolve('alias');
        })->toThrow(NotFoundException::class);
    }
}
