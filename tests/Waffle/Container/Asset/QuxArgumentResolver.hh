<?hh

namespace Waffle\Tests\Container\Asset;

use type Waffle\Container\Argument\ArgumentResolverInterface;
use type Waffle\Container\Argument\ArgumentResolverTrait;
use type Waffle\Container\ContainerAwareTrait;

class QuxArgumentResolver implements ArgumentResolverInterface
{
    use ContainerAwareTrait;
    use ArgumentResolverTrait;
}
