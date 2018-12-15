<?hh // strict

namespace Waffle\Container\Argument;

class RawArgument implements RawArgumentInterface
{
    protected mixed $value;

    public function __construct(mixed $value)
    {
        $this->value = $value;
    }

    public function getValue(): mixed
    {
        return $this->value;
    }
}
