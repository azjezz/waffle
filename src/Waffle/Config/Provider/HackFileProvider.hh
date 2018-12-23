<?hh // strict

namespace Waffle\Config\Provider;

use namespace Waffle\Config\Exception;
use type Generator;
use function file_exists;

class HackFileProvider extends MultipleResourcesProvider
{
    use GlobTrait;

    public function __construct(
        private string $pattern
    ) {}

    public function loadMany(): Generator<string, KeyedContainer<arraykey, mixed>, void>
    {
        foreach ($this->glob($this->pattern) as $file) {
            $return = include $file;
            /* HH_IGNORE_ERROR[4110] */
            yield $file => $return as KeyedContainer<_, _>;
        }
    }
}
