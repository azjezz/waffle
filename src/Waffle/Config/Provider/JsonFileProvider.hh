<?hh // strict

namespace Waffle\Config\Provider;

use type Waffle\Json\Json;
use type Generator;
use function file_get_contents;

class JsonFileProvider extends MultipleResourcesProvider
{
    use GlobTrait;

    public function __construct(
        private string $pattern
    ) {}

    <<__Override>>
    protected function loadMany(): Generator<string, KeyedContainer<arraykey, mixed>, void>
    {
        foreach ($this->glob($this->pattern) as $file) {
            $return = Json::decode(file_get_contents($file), true);
            /* HH_IGNORE_ERROR[4110] */
            yield $file => $return as KeyedContainer<_, _>;
        }
    }
}
