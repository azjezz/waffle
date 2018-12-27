<?hh // strict

namespace Waffle\Mix\Recipe;

use type Waffle\Http\Session\SessionOptions;
use type Waffle\Http\Session\SessionMiddleware;
use type Waffle\Http\Session\Persistence\CacheSessionPersistence;

class SessionRecipe extends Recipe
{
    <<__Override>>
    public function mix(): void
    {
        $options = $this->config()->get('session', shape()) as SessionOptions;

        $persistence = $options['persistence'] ?? null;

        if ($persistence === CacheSessionPersistence::class) {
            $this->require(CacheRecipe::class);
        }

        $this->pipe(SessionMiddleware::class, 0x09900);
    }
}
