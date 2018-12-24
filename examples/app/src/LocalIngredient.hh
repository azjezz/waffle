<?hh // strict

namespace Example\App;

use Waffle\Mix\Ingredient;

class LocalIngredient extends Ingredient
{
    public function mix(): void
    {
        $this->share(LocalHandler::class);
    }
}