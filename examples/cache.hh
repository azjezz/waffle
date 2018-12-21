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

    $serializer = new Cache\Serializer\DefaultSerializer();
    
    // $store = new Cache\Store\ApcuStore($serializer);
    
    $redis = new Redis();
    $redis->connect('localhost');
    
    $namespacedStore = new Cache\Store\RedisStore($redis, 'example', 3600, $serializer);
    $store = new Cache\Store\RedisStore($redis, '', 3600, $serializer);

    $namespacedPool = new Cache\CacheItemPool($namespacedStore);
    $pool = new Cache\CacheItemPool($store);

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
    
    assert( $pool->getItem($key)->get() === 'Hack Lang');

    assert( $pool->clear() );

    assert( $pool->hasItem($key) === false);

}
