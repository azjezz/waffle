<?hh

namespace Waffle\Tests\Container\Asset;

use type Waffle\Container\ContainerAwareInterface;
use type Waffle\Container\ContainerAwareTrait;

class BarContainerAware implements ContainerAwareInterface
{
    use ContainerAwareTrait;

    protected $something;

    public function setSomething($something)
    {
        $this->something = $something;
    }
}
