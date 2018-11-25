<?hh // strict

namespace Waffle\Container;

use Waffle\Contract\Container\ContainerInterface;

interface ContainerAwareInterface
{
    /**
     * Set a container
     */
    public function setContainer(ContainerInterface $container): this;

    /**
     * Get the container
     */
    public function getContainer(): ContainerInterface;
}
