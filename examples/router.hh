<?hh // strict

namespace Examples\Router;

use namespace Waffle\Http;
use namespace Waffle\Router;
use namespace Waffle\Contract\Http\Server;
use namespace Waffle\Contract\Http\Message;
use function assert;

require_once '../vendor/hh_autoload.php';


class DummyMiddleware implements Server\MiddlewareInterface
{
    public function __construct(
        private string $message = 'Hello'
    ) {}

    public function process(Message\ServerRequestInterface $_, Server\RequestHandlerInterface $_): Message\ResponseInterface
    {
        return new Http\Message\Response\TextResponse($this->message);
    }
}

<<__EntryPoint>>
function main(): void
{
    $factory = new Http\Message\Factory();

    $router = new Router\Router();
    $collector = new Router\RouteCollector($router);

    $home       = $collector->get('/home', new DummyMiddleware('home, sweet home !'));
    $usersApi   = $collector->delete('/api/users/{username}', new DummyMiddleware('delete that user'));
    $any        = $collector->any('/any', new DummyMiddleware('any method works with me !'));

    $result = $router->match(
        $factory->createRequest('DELETE', $factory->createUri('/api/users/azjezz'))
    );

    assert($result->isSuccess());
    assert($result->getMatchedRoute() === $usersApi);
    assert($result->getMatchedParams()['username'] === 'azjezz');

    $result = $router->match(
        $factory->createRequest('GET', $factory->createUri('/home'))
    );

    assert($result->isSuccess());
    assert($result->getMatchedRoute() === $home);

    $result = $router->match(
        $factory->createRequest('POST', $factory->createUri('/home'))
    );

    assert($result->isMethodFailure());
    assert($result->getMatchedRoute() === null);

    $result = $router->match(
        $factory->createRequest('DOIT', $factory->createUri('/any'))
    );

    assert($result->isSuccess());
    assert($result->getMatchedRoute() === $any);
}