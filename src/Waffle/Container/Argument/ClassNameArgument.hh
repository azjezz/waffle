<?hh // strict

namespace Waffle\Container\Argument;

class ClassNameArgument implements ClassNameArgumentInterface
{
    public function __construct(
        protected string $value
    ) {
        $this->value = $value;
    }

    public function getValue(): string
    {
        return $this->value;
    }
}
