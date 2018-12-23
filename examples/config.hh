<?hh // strict

namespace Examples\Config;

use namespace Waffle\Config;
use function realpath;

require __DIR__.'/../vendor/hh_autoload.hh';

<<__EntryPoint>>
function main(): noreturn
{
    $container = new Config\Provider\ContainerProvider([
        Config\ConfigAggregator::ENABLE_CACHE => true,
    ]);
    $ini  = new Config\Provider\IniFileProvider(realpath(__DIR__) . '/autoload/*.ini');
    $hack = new Config\Provider\HackFileProvider(realpath(__DIR__) . '/autoload/{{,*.}global,{,*.}local}.hh');
    $json = new Config\Provider\JsonFileProvider(realpath(__DIR__) . '/autoload/{{,*.}global,{,*.}local}.json');

    $config = new Config\Configuration(vec[
        $container, $ini, $hack, $json
    ], __DIR__ . '/cache/cache.hh');

    \var_dump($config->items());

    exit(0);
}