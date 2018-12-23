<?hh // strict

namespace Waffle\Config\Provider;

use type Generator;
use function parse_ini_file;
use const INI_SCANNER_NORMAL;

class IniFileProvider extends MultipleResourcesProvider
{
    use GlobTrait;

    public function __construct(
        protected string $pattern,
        protected bool $processSections = true,
        protected int $scannerMode = INI_SCANNER_NORMAL
    ) {}

    <<__Override>>
    protected function loadMany(): Generator<string, KeyedContainer<arraykey, mixed>, void>
    {
        foreach ($this->glob($this->pattern) as $file) {
            yield $file => parse_ini_file($file, $this->processSections, $this->scannerMode);
        }
    }
}
