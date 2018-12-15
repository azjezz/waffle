<?hh

namespace Waffle\Tests\Container\Asset;

use Waffle\Container\ContainerAwareInterface;
use Waffle\Container\ContainerAwareTrait;

class BarContainerAware implements ContainerAwareInterface 
{
    use ContainerAwareTrait;

    protected $something;

    public function setSomething($something)
    {
        $this->something = $something;
    }
}
