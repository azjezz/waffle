<?hh // strict

namespace Waffle\Container;

use Waffle\Container\Exception\ContainerException;
use Waffle\Contract\Container\ContainerInterface;

trait ContainerAwareTrait
{
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