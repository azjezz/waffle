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

    $redis = new Redis();
    $redis->connect('localhost');
    $serializer = new Cache\Serializer\DefaultSerializer();
    $store = new Cache\Store\RedisStore($redis, 3600, $serializer);
    $pool = new Cache\CacheItemPool($store);

    $item = $pool->getItem($key);
    $item2 = $pool->getItem($key . '2');

    assert( $item->isHit() === false );

    $item->set('Hack Lang');
    $item2->set('PHP');
    $item2->expiresAfter(new \DateInterval('PT5S'));

    assert( $pool->save($item) );
    assert( $pool->save($item2) );


    assert( $pool->hasItem($key) );
    assert( $pool->hasItem($key.'2') );

    sleep(6);
    // delete expired items ~
    assert( $pool->prune() );

    assert( $pool->hasItem($key) );
    assert( $pool->hasItem($key.'2') === false);

    assert( $pool->getItem($key)->get() === 'Hack Lang');
}
