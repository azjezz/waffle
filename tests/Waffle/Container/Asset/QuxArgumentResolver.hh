<?hh

namespace Waffle\Tests\Container\Asset;

use Waffle\Container\Argument\ArgumentResolverInterface;
use Waffle\Container\Argument\ArgumentResolverTrait;
use Waffle\Container\ContainerAwareTrait;

class QuxArgumentResolver implements ArgumentResolverInterface
{
    use ContainerAwareTrait;
    use ArgumentResolverTrait;
}