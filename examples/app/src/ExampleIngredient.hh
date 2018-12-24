<?hh // strict

namespace Example\App;

use Waffle\Mix\Ingredient;

class ExampleIngredient extends Ingredient
{
    public function mix(): void
    {
        $this->share(ExampleHandler::class);
    }
}