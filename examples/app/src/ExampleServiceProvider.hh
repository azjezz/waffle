<?hh // strict

namespace Example\App;

use type Waffle\Container\ServiceProvider\AbstractServiceProvider;

class ExampleServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        Handler\DataHandler::class,
        Handler\PingHandler::class,
        Handler\HomeHandler::class,
        Middleware\DataMiddleware::class
    ];

    <<__Override>>
    public function register(): void
    {
        $this->share(Handler\HomeHandler::class);
        $this->share(Handler\DataHandler::class);
        $this->share(Handler\PingHandler::class);
        $this->share(Middleware\DataMiddleware::class);
    }
}
