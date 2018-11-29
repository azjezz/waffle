<?hh // strict

namespace Waffle\Container;

use type Waffle\Container\Exception\ContainerException;
use type Waffle\Contract\Container\ContainerInterface;

trait ContainerAwareTrait
{
    require implements ContainerAwareInterface;

    protected ?ContainerInterface $container;

    /**
     * Set a container.
     */
    public function setContainer(ContainerInterface $container): this
    {
        $this->container = $container;
        return $this;
    }

    /**
     * Get the container.
     */
    public function getContainer(): ContainerInterface
    {
        if ($this->container instanceof ContainerInterface) {
            return $this->container;
        }

        throw new ContainerException('No container implementation has been set.');
    }
}
