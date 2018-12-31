<?hh // strict

namespace Waffle\Security\Password;

use const PASSWORD_BCRYPT;

class BcryptPasswordHasher extends NativePasswordHasher
{
    public function __construct(
        protected int $cost = 8
    ) {}

    protected function getAlgorithm(): int
    {
        return PASSWORD_BCRYPT;
    }

    protected function getOptions(): array<string, mixed>
    {
        return [
            'cost' => $this->cost
        ];
    }
}