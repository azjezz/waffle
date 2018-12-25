<?hh // strict

namespace Example\App;

use type Waffle\Container\ServiceProvider\AbstractServiceProvider;

class ExampleServiceProvider extends AbstractServiceProvider
{
    public function register(): void
    {
        $this->share(Handler\HomeHandler::class);
        $this->share(Handler\DataHandler::class);
        $this->share(Handler\PingHandler::class);
        $this->share(Middleware\DataMiddleware::class);
    }
}
