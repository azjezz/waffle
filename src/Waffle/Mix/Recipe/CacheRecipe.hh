<?hh // strict

namespace Waffle\Mix\Recipe;

use type Waffle\Mix\ServiceProvider\CacheServiceProvider;
use type Waffle\Mix\ServiceProvider\SimpleCacheServiceProvider;

class CacheRecipe extends Recipe
{
    <<__Override>>
    public function mix(): void
    {
        $this->register(new CacheServiceProvider());
        $this->register(new SimpleCacheServiceProvider());
    }
}
