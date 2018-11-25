<?hh // strict

namespace Waffle\Container\ServiceProvider;

use Generator;
use Waffle\Container\{ContainerAwareInterface, ContainerAwareTrait};
use Waffle\Container\Exception\ContainerException;
use function is_string;
use function class_exists;
use function sprintf;
use ReflectionClass;

class ServiceProviderAggregate implements ServiceProviderAggregateInterface
{
    use ContainerAwareTrait;

    protected Vector<ServiceProviderInterface> $providers;

    protected Set<string> $registered;


    public function __construct()
    {
        $this->providers = Vector {};
        $this->registered = Set {};
    }

    /**
     * {@inheritdoc}
     */
    public function add(mixed $provider): this
    {
        if (is_string($provider) && $this->getContainer()->has($provider)) {
            $provider = $this->getContainer()->get($provider);
        } elseif (is_string($provider) && class_exists($provider)) {
            $reflection = new ReflectionClass($provider);
            $provider = $reflection->newInstance();
        }

        if ($provider instanceof ContainerAwareInterface) {
            $provider->setContainer($this->getContainer());
        }

        if ($provider instanceof BootableServiceProviderInterface) {
            $provider->boot();
        }

        if ($provider instanceof ServiceProviderInterface) {
            $this->providers[] = $provider;

            return $this;
        }

        throw new ContainerException(
            'A service provider must be a fully qualified class name or instance ' .
            'of (\Waffle\Container\ServiceProvider\ServiceProviderInterface)'
        );
    }

    /**
     * {@inheritdoc}
     */
    public function provides(string $service): bool
    {
        foreach ($this->getIterator() as $provider) {
            if ($provider->provides($service)) {
                return true;
            }
        }

        return false;
    }

    /**
     * {@inheritdoc}
     */
    public function getIterator(): Iterator<ServiceProviderInterface>
    {   
        return $this->providers->getIterator();
    }

    /**
     * {@inheritdoc}
     */
    public function register(string $service): void
    {
        if (! $this->provides($service)) {
            throw new ContainerException(
                sprintf('(%s) is not provided by a service provider', $service)
            );
        }

        foreach ($this->getIterator() as $provider) {
            if ($this->registered->contains($provider->getIdentifier())) {
                continue;
            }

            if ($provider->provides($service)) {
                $provider->register();
                $this->registered[] = $provider->getIdentifier();
            }
        }
    }
}