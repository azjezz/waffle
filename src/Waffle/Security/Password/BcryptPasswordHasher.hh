<?hh // strict

namespace Waffle\Security\Password;

use const PASSWORD_BCRYPT;

class BcryptPasswordHasher extends NativePasswordHasher
{
    public function __construct(
        protected int $cost = 8
    ) {}

    <<__Override>>
    protected function getAlgorithm(): int
    {
        return PASSWORD_BCRYPT;
    }

    <<__Override>>
    protected function getOptions(): array<string, mixed>
    {
        return [
            'cost' => $this->cost
        ];
    }
}
