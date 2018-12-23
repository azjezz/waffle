<?hh // strict

namespace Examples\Config;

use namespace Waffle\Config;
use function realpath;

require __DIR__.'/../vendor/hh_autoload.hh';

<<__EntryPoint>>
function main(): noreturn
{
    $container = new Config\Provider\ContainerProvider([
        Config\ConfigAggregator::ENABLE_CACHE => false,
    ]);
    $hack = new Config\Provider\HackFileProvider(realpath(__DIR__) . '/autoload/{{,*.}global,{,*.}local}.hh');
    $json = new Config\Provider\JsonFileProvider(realpath(__DIR__) . '/autoload/{{,*.}global,{,*.}local}.json');

    $aggregator = new Config\ConfigAggregator(vec[
        $container, $hack, $json
    ], __DIR__ . '/cache/cache.hh');

    $config = new Config\Configuration($aggregator->get());

    \var_dump(
        $config->items()->firstValue()?->lastValue(), 
        // same as this
        $config->get('a')
    );
    // "a" is the first pair in the items, the last value in `a pair` is the value of "a" where the first value is "a" ( the key )
    exit(0);
}