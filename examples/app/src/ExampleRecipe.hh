<?hh // strict

namespace Example\App;

use Waffle\Mix\Recipe\Recipe;

class ExampleRecipe extends Recipe
{
    public function mix(): void
    {
        $this->share(ExampleHandler::class);
    }
}