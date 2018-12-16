<?hh // strict

namespace Waffle\Config;

use type Waffle\Contract\Config\ConfigurationInterface;
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;

class ConfigServiceProvider extends AbstractServiceProvider
{
    public function __construct(
        private dict<string, string> $files
    ) {
        parent::__construct();
    }

    protected vec<string> $provides = vec[
        ConfigurationInterface::class,
    ];

    public function register(): void
    {
        $this->share(
            ConfigurationInterface::class,
            (): ConfigurationInterface ==> {
                $items = dict[];
                foreach ($this->files as $key => $file) {
                    $items[$key] = require $file;
                }
                return new Configuration($items);
            }
        );
    }
}
