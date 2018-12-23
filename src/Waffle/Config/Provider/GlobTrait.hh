<?hh // strict

namespace Waffle\Config\Provider;

use function glob;
use const GLOB_BRACE;

trait GlobTrait
{
    private function glob(string $pattern): Container<string>
    {
        return glob($pattern, GLOB_BRACE);
    }
}
