<?hh // strict

namespace Waffle\Cache\Store;

use namespace HH\Lib\C;
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

class ApcuStore implements StoreInterface
{
    public function __construct(
        protected SerializerInterface $serializer = new DefaultSerializer()
    ) {}

    public function store(string $id, mixed $value, num $ttl = 0): bool
    {
        return apc_store($id, $this->serializer->serialize($value), $ttl);
    }

    public function contains(string $id): bool
    {
        return apc_exists($id);
    }

    public function delete(string $id): bool
    {
        if (!$this->contains($id)) {
            return false;
        }

        return apc_delete($id);
    }

    public function get(string $id): mixed
    {
        if (!$this->contains($id)) {
            return null;
        }

        return $this->serializer->unserialize(
            (string) apc_fetch($id)
        );
    }

    public function clear(string $namespace): bool
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