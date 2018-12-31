<?hh // strict

namespace Waffle\Security\Password;

use type Waffle\Contract\Security\Password\PasswordHasherInterface;

use type Waffle\Lib\Shape;
use function password_get_info;
use function password_hash;
use function password_needs_rehash;
use function password_verify;
use const PASSWORD_DEFAULT;

abstract class NativePasswordHasher implements PasswordHasherInterface
{
    public function hash(string $password): string
    {
        return password_hash(
            $password,
            $this->getAlgorithm(),
            $this->getOptions()
        ) as string;
    }

    public function match(string $hash): bool
    {
        return password_needs_rehash(
            $hash,
            $this->getAlgorithm(),
            $this->getOptions()
        );
    }

    /**
     * Returns information about the given hash.
     *
     * if the supplied hash doesn't belong to the current
     * password hasher, this method MUST return null.
     *
     * @see match(strign $hash)
     */
    public function info(string $hash): ?shape(...)
    {
        if (!$this->match($hash)) {
            return null;
        }
        return Shape::create(
            password_get_info($hash)
        );
    }

    public function check(string $password, string $hash): bool
    {
        return password_verify($password, $hash);
    }

    abstract protected function getAlgorithm(): int;

    abstract protected function getOptions(): array<string, mixed>;
}
