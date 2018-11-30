<?hh

namespace Waffle\Tests\Container\ServiceProvider;

use Facebook\HackTest\HackTest;
use Waffle\Container\Container;
use Waffle\Container\ContainerAwareTrait;
use Waffle\Container\Exception\ContainerException;
use Waffle\Tests\Container\Asset\FakeServiceProvider;
use Waffle\Container\ServiceProvider\ServiceProviderAggregate;
use function Facebook\FBExpect\expect;

class ServiceProviderAggregateTest extends HackTest
{
    /**
     * Return a service provider fake
     *
     * @return \Waffle\Container\ServiceProvider\ServiceProviderInterface
     */
    protected function getServiceProvider(): FakeServiceProvider
    {
        return new FakeServiceProvider();
    }

    /**
     * Asserts that the aggregate adds a class name service provider.
     */
    public function testAggregateAddsClassNameServiceProvider()
    {
        $container = new Container();
        $aggregate = (new ServiceProviderAggregate())->setContainer($container);

        $aggregate->add($this->getServiceProvider());

        expect($aggregate->provides('SomeService'))->toBeTrue();
        expect($aggregate->provides('AnotherService'))->toBeTrue();
    }

    /**
     * Asserts that an exception is thrown when adding a service provider that
     * does not exist.
     */
    public function testAggregateThrowsWhenCannotResolveServiceProvider()
    {
        expect(() ==> {
            $container = new Container();
            $aggregate = (new ServiceProviderAggregate())->setContainer($container);
            $aggregate->add('NonExistentClass');
        })->toThrow(ContainerException::class);
    }

    /**
     * Asserts that an exception is thrown when attempting to invoke the register
     * method of a service provider that has not been provided.
     */
    public function testAggregateThrowsWhenRegisteringForServiceThatIsNotAdded()
    {
        expect(() ==> {
            $container = new Container();
            $aggregate = (new ServiceProviderAggregate())->setContainer($container);
            $aggregate->register('SomeService');
        })->toThrow(ContainerException::class);
    }

    /**
     * Asserts that resgister method is only invoked once per service provider.
     */
    public function testAggregateInvokesCorrectRegisterMethodOnlyOnce()
    {
        $container = new Container();
        $aggregate = (new ServiceProviderAggregate())->setContainer($container);
        $provider  = $this->getServiceProvider();

        $aggregate->add($provider);

        $aggregate->register('SomeService');
        $aggregate->register('AnotherService');

        if ($provider is FakeServiceProvider) {
            expect($provider->registered)->toBeSame(1);
        } else {
            static::fail('Expected ServiceProviderAggregate to return FakeServiceProvider.');
        }
    }
}
