<?hh // strict

namespace Waffle\Config\Provider;

use type Generator;

class HackFileProvider extends MultipleResourcesProvider
{
    use GlobTrait;

    public function __construct(
        private string $pattern
    ) {}

    <<__Override>>
    public function loadMany(): Generator<string, KeyedContainer<arraykey, mixed>, void>
    {
        foreach ($this->glob($this->pattern) as $file) {
            $return = include $file;
            /* HH_IGNORE_ERROR[4110] */
            yield $file => $return as KeyedContainer<_, _>;
        }
    }
}
