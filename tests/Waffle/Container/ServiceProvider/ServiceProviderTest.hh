<?hh 

namespace Waffle\Tests\Container\ServiceProvider;

use type Waffle\Container\ServiceProvider\ServiceProviderInterface;
use type Waffle\Container\Container;
use type Waffle\Tests\Container\Asset\FakeServiceProvider;
use type Facebook\HackTest\HackTest;
use function Facebook\FBExpect\expect;

class ServiceProviderTest extends HackTest
{

    /**
     * Return a service provider fake
     *
     * @return \Waffle\Container\ServiceProvider\ServiceProviderInterface
     */
    protected function getFakeServiceProvider(): ServiceProviderInterface
    {
        return new FakeServiceProvider();
    }

    /**
     * Asserts that the service provider correctly determines what it provides.
     */
    public function testServiceProviderCorrectlyDeterminesWhatIsProvided(): void
    {
        $provider = $this->getFakeServiceProvider();
        $provider->setIdentifier('something');
        expect($provider->provides('SomeService'))->toBeTrue();
        expect($provider->provides('AnotherService'))->toBeTrue();
        expect($provider->provides('NonService'))->toBeFalse();
    }
}
