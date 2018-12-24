<?hh // strict

namespace Example\App;

use namespace Waffle\Mix;
use namespace Waffle\Config;

require __DIR__ .'/../../../vendor/hh_autoload.hh';

<<__EntryPoint>>
async function main(): Awaitable<void>
{
    $configuration = new Config\Configuration(vec[]);
    $application = new Mix\Application(
        Mix\Environment::development(/* $debug = false  | default = true in dev-mode */),
        $configuration
    );
    $application->use(LocalIngredient::class);
    $application->get('/', LocalHandler::class);
    $application->get('/{something}', LocalHandler::class);
    $application->run();
}