<?hh

namespace Waffle\Tests\Container\Inflector;

use namespace HH\Lib\C;
use type Waffle\Container\Inflector\Inflector;
use type Waffle\Container\Container;
use type Facebook\HackTest\HackTest;
use type Waffle\Tests\Container\Asset\Bar;
use type Waffle\Tests\Container\Asset\Baz;
use type Waffle\Tests\Container\Asset\Foo;
use function Facebook\FBExpect\expect;

class InflectorTest extends HackTest
{
    /**
     * Asserts that the inflector sets expected method calls.
     */
    public function testInflectorSetsExpectedMethodCalls()
    {
        $container = new Container();
        $inflector = (new Inflector('Type'))->setContainer($container);

        $inflector->invokeMethod('method1', vec[ 'arg1' ]);

        $inflector->invokeMethods(dict[
            'method2' => vec[ 'arg1' ] ,
            'method3' => vec[ 'arg1' ]
        ]);

        $methods = (new \ReflectionClass($inflector))->getProperty('methods');
        $methods->setAccessible(true);

        $methods = $methods->getValue($inflector);

        expect(C\contains_key($methods, 'method1'))->toBeTrue();
        expect(C\contains_key($methods, 'method2'))->toBeTrue();
        expect(C\contains_key($methods, 'method3'))->toBeTrue();

        expect($methods['method1'])->toBeSame(vec['arg1']);
        expect($methods['method2'])->toBeSame(vec['arg1']);
        expect($methods['method3'])->toBeSame(vec['arg1']);
    }

    /**
     * Asserts that the inflector sets expected properties.
     */
    public function testInflectorSetsExpectedProperties()
    {
        $container = new Container();
        $inflector = (new Inflector('Type'))->setContainer($container);

        $inflector->setProperty('property1', 'value');

        $inflector->setProperties(dict[
            'property2' => 'value',
            'property3' => 'value'
        ]);

        $properties = (new \ReflectionClass($inflector))->getProperty('properties');
        $properties->setAccessible(true);

        $dict = $properties->getValue($inflector);

        expect(C\contains_key($dict, 'property1'))->toBeTrue();
        expect(C\contains_key($dict, 'property2'))->toBeTrue();
        expect(C\contains_key($dict, 'property3'))->toBeTrue();

        expect($dict['property1'])->toBeSame('value');
        expect($dict['property2'])->toBeSame('value');
        expect($dict['property3'])->toBeSame('value');
    }

    /**
     * Asserts that the inflector will inflect on an object with properties.
     */
    public function testInflectorInflectsWithProperties()
    {
        $bar = new Bar();

        $container = new Container();
        $container->add(Bar::class, (): Bar ==> $bar);

        $inflector = (new Inflector('Type'))
            ->setContainer($container)
            ->setProperty('bar', Bar::class)
        ;

        $baz = new Baz();

        $inflector->inflect($baz);

        expect($baz->bar)->toBeInstanceOf(Bar::class);
        expect($baz->bar)->toBeSame($bar);
    }

    /**
     * Asserts that the inflector will inflect on an object with method call.
     */
    public function testInflectorInflectsWithMethodCall()
    {
        $container = new Container();

        $bar = new Bar();

        $container->add(Bar::class, (): Bar ==> $bar);

        $inflector = (new Inflector('Type'))
            ->setContainer($container)
            ->invokeMethod('setBar', vec[
                Bar::class
            ])
        ;

        $foo = new Foo(null);

        $inflector->inflect($foo);

        expect($foo->bar)->toBeInstanceOf(Bar::class);
        expect($foo->bar)->toBeSame($bar);
    }

    /**
     * Asserts that the inflector will inflect on an object with a callback.
     */
    public function xtestInflectorInflectsWithCallback()
    {
        $foo = new Foo(null);

        $bar = new Bar();

        $inflector = new Inflector('Type', (mixed $object): void ==> {
            /* HH_IGNORE_ERROR[4064] x */
            $object->setBar($bar);
        });

        $inflector->inflect($foo);

        expect($foo->bar)->toBeSame($bar);
    }
}
