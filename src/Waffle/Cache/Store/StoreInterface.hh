<?hh // strict

namespace Waffle\Cache\Store;

use type Waffle\Contract\Service\ResetInterface;

interface StoreInterface extends ResetInterface
{
    public function store(string $id, mixed $value, num $ttl = 0): bool;

    public function contains(string $id): bool;

    public function delete(string $id): bool;

    public function get(string $id): mixed;

    public function prune(): bool;
}
