<?hh // strict

namespace Example\App;

use Waffle\Mix\Recipe\Recipe;

class ExampleRecipe extends Recipe
{
    public function mix(): void
    {
        $this->register(new ExampleServiceProvider());

        $this->get('/', Handler\HomeHandler::class, 'home');

        $this->get('/data', dict([
            Middleware\DataMiddleware::class,
            Handler\DataHandler::class
        ]), 'data');

        $this->get('/ping', Handler\PingHandler::class, 'ping');
    }
}
