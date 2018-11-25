<?hh // strict

namespace Waffle\Container\ServiceProvider;

use Waffle\Container\ContainerAwareTrait;
use function get_class;

abstract class AbstractServiceProvider implements ServiceProviderInterface
{
    use ContainerAwareTrait;

    protected Set<string> $provides;

    protected string $identifier;

    public function __construct()
    {
        $this->provides = Set {};
        $this->identifier = get_class($this);
    }

    /**
     * {@inheritdoc}
     */
    public function provides(string $alias): bool
    {
        return $this->provides->contains($alias);
    }

    /**
     * {@inheritdoc}
     */
    public function setIdentifier(string $id): ServiceProviderInterface
    {
        $this->identifier = $id;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getIdentifier(): string
    {
        return $this->identifier;
    }
}
