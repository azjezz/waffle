<?hh // strict

namespace Waffle\Cache\Store;

use namespace HH\Lib\Str;
use namespace HH\Lib\C;
use function microtime;

type Item = shape(
    'value' => mixed,
    'expiry' => float
);

class ArrayStore implements StoreInterface
{
    public function __construct(
        protected dict<string, Item> $cache = dict[]
    ) {}

    public function store(string $id, mixed $value, num $ttl = 0): bool
    {
        $expire =
        $this->cache[$id] = shape(
            'value' => $value,
            'expiry' => 0 === $ttl ? 0.0 : microtime(true) + $ttl
        );

        return true;
    }

    public function contains(string $id): bool
    {
        if (!C\contains_key($this->cache, $id)) {
            return false;
        }

        $expiry = $this->cache[$id]['expiry'];

        if (0.0 === $expiry) {
            return true;
        }

        $expired = $expiry <= microtime(true);

        if ($expired) {
            unset($this->cache[$id]);
        }

        return !$expired;
    }

    public function delete(string $id): bool
    {
        if (!$this->contains($id)) {
            return false;
        }

        unset($this->cache[$id]);
        return true;
    }

    public function get(string $id): mixed
    {
        if (!$this->contains($id)) {
            return null;
        }

        return $this->cache[$id]['value'];
    }

    public function clear(string $namespace): bool
    {
        $ok = true;
        foreach ($this->cache as $key => $item) {
            if (Str\is_empty($namespace) || Str\starts_with($key, $namespace.'.')) {
                $ok = $this->delete($key) && $ok;
            }
        }
        return $ok;
    }
}
