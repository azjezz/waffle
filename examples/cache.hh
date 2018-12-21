<?hh // strict

namespace Examples\Cache;

use namespace Waffle\Cache;
use type Redis;
use function sleep;
use function bin2hex;
use function random_bytes;

require '../vendor/hh_autoload.hh';

<<__EntryPoint>>
function main(): void
{
    $key = bin2hex(random_bytes(16));

    // $serializer = new Cache\Serializer\DefaultSerializer();
    
    // $store = new Cache\Store\ApcuStore($serializer);
    
    // $redis = new Redis();
    // $redis->connect('localhost');
    // $store = new Cache\Store\RedisStore($redis, $serializer);

    $store = new Cache\Store\ArrayStore();

    $namespacedPool = new Cache\CacheItemPool($store, 3600, 'example');
    $pool = new Cache\CacheItemPool($store, 3600);

    $item = $pool->getItem($key);

    assert( $item->isHit() === false );

    $item->set('Hack Lang');

    assert( $pool->save($item) );
    assert( $namespacedPool->save($item) );

    assert( $pool->hasItem($key) );
    assert( $namespacedPool->hasItem($key) );

    assert( $namespacedPool->clear() );

    assert( $pool->hasItem($key) );
    assert( $namespacedPool->hasItem($key) === false);
}
