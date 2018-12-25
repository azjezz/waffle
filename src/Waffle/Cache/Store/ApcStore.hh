<?hh // strict

namespace Waffle\Cache\Store;

use namespace HH\Lib\Str;
use type Waffle\Cache\Serializer\SerializerInterface;
use type Waffle\Cache\Serializer\DefaultSerializer;
use type APCIterator;
use function apc_store;
use function apc_exists;
use function apc_delete;
use function apc_fetch;
use function apc_clear_cache;
use function preg_quote;
use const APC_ITER_KEY;
use const PHP_SAPI;

class ApcStore extends Store
{
    public function __construct(
        string $namespace = '',
        num $defaultTtl = 0,
        protected SerializerInterface $serializer = new DefaultSerializer(),
    ) {
        parent::__construct($namespace, $defaultTtl);
    }

    <<__Override>>
    public function set(string $id, mixed $value, num $ttl = 0): bool
    {
        return apc_store($id, $this->serializer->serialize($value), $ttl);
    }

    <<__Override>>
    public function has(string $id): bool
    {
        return apc_exists($id);
    }

    <<__Override>>
    public function remove(string $id): bool
    {
        return apc_delete($id);
    }

    <<__Override>>
    public function retrieve(string $id): mixed
    {
        if (!$this->has($id)) {
            return null;
        }

        return $this->serializer->unserialize(
            (string) apc_fetch($id)
        );
    }

    <<__Override>>
    public function wipe(string $namespace): bool
    {
        if (Str\is_empty($namespace)) {
            return apc_clear_cache();
        }

        /* HH_IGNORE_ERROR[2049] */
        $iterator = new APCIterator(Str\format(
            '/^%s/',
            preg_quote($namespace, '/')
        /* HH_IGNORE_ERROR[2049] */
        /* HH_IGNORE_ERROR[4106] */
        ), APC_ITER_KEY);

        return apc_delete($iterator);
    }
}
