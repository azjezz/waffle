<?hh // strict

namespace Waffle\Contract\Security\Password;

interface PasswordHasherInterface
{
    /**
     * Create a password hash.
     */
    public function hash(string $password): string;

    /**
     * Check if the given hash matches the given password.
     */
    public function check(string $password, string $hash): bool;

    /**
     * Check if the given hash matchs the password hasher
     * algorithm.
     */
    public function match(string $hash): bool;

    /**
     * Returns information about the given hash.
     *
     * if the supplied hash doesn't belong to the current
     * password hasher, this method MUST return null.
     *
     * @see match(strign $hash)
     */
    public function info(string $hash): ?shape(...);
}
