<?hh // strict

namespace Example\App;

use namespace Waffle\Mix;
use namespace Waffle\Config;

require __DIR__ .'/../../../vendor/hh_autoload.hh';

<<__EntryPoint>>
function main(): void
{
    $configuration = new Config\Configuration(vec[
        new Config\Provider\ContainerProvider(dict[
            'session' => shape(
                'cookie' => shape(
                    'name' => 'sessy',
                    'lifetime' => 60 * 60 * 24 * 7 /* 1 week */
                ),
                'persistence' => 'native'
            )
        ])
    ]);

    $application = new Mix\Application(
        Mix\Environment::development(/* $debug = false  | default = true in dev-mode */),
        $configuration
    );

    $application->use(Mix\Recipe\SessionRecipe::class);
    $application->use(ExampleRecipe::class);
    $application->run();
}
