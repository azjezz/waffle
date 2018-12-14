<?hh

namespace Waffle\Tests\Container\Inflector;

use Waffle\Container\ContainerAwareInterface;
use Waffle\Container\Container;
use function Facebook\FBExpect\expect;
use Waffle\Container\Inflector\InflectorAggregate;
use Waffle\Container\Inflector\Inflector;
use Facebook\HackTest\HackTest;
use Waffle\Contract\Container\ContainerInterface;
use Waffle\Tests\Container\Asset\BarContainerAware;

class InflectorAggregateTest extends HackTest
{
    /**
     * Asserts that the aggregate can add an inflector.
     */
    public function testAggregateAddsInflector()
    {
        $aggregate = new InflectorAggregate();
        $inflector = $aggregate->add('Some\Type');

        expect($inflector)->toBeInstanceOf(Inflector::class);
        expect($inflector->getType())->toBeSame('Some\Type');
    }

    /**
     * Asserts that the aggregate adds and iterates multiple inflectors.
     */
    public function testAggregateAddsAndIteratesMultipleInflectors()
    {
        $aggregate  = new InflectorAggregate();
        $inflectors = [];

        for ($i = 0; $i < 10; $i++) {
            $inflectors[] = $aggregate->add('Some\Type' . $i);
        }

        foreach ((array) $aggregate->getIterator() as $key => $inflector) {
            expect($inflector)->toBeSame($inflectors[$key]);
        }
    }

    /**
     * Asserts that the aggregate iterates and inflects on an object.
     */
    public function testAggregateIteratesAndInflectsOnObject()
    {
        $aggregate      = new InflectorAggregate();
        $container = new Container();
        $aggregate->setContainer($container);

        $aggregate->add(ContainerAwareInterface::class)
            ->invokeMethod('setContainer', vec[ $container ]);

        $aggregate->add('Ignored\Type');

        $containerAware = new BarContainerAware();
        $aggregate->inflect($containerAware);
        expect($containerAware->getContainer())->toBeSame($container);
    }
}
