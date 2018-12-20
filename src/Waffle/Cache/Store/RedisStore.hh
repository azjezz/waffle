<?hh // strict

namespace Waffle\Cache\Store;

use type Waffle\Cache\Serializer\SerializerInterface;
use type Waffle\Cache\Serializer\DefaultSerializer;
use type Redis;

class RedisStore implements StoreInterface
{
    public function __construct(
        protected Redis $redis,
        protected SerializerInterface $serializer = new DefaultSerializer()
    ) {
        $redis->ping();
    }

    public function get(string $key): mixed
    {
        if (!$this->contains($key)) {
            return null;
        }

        return $this->serializer->unserialize(
            (string) $this->redis->get($key)
        );
    }

    public function delete(string $key): bool
    {
        $this->redis->del($key);
        return true;
    }

    public function contains(string $key): bool
    {
        return (bool) $this->redis->exists($key);
    }

    public function store(string $id, mixed $value, num $ttl = 0): bool
    {
        $value = $this->serializer->serialize($value);

        if (0 >= $ttl) {
            return (bool) $this->redis->set($id, $value);
        } else {
            return (bool) $this->redis->setex($id, $ttl, $value);
        }
    }

    public function prune(): bool
    {
        /*
         * Redis deletes items automatically after the timeout has expired.
         */
        return true;
    }

    public function reset(): void
    {
        // noop
    }
}
