<?hh // strict

namespace Waffle\Http\Server\Middleware;

use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Container\ContainerInterface;
use type Waffle\Http\Server\MiddlewareFactory;

class LazyMiddlewareDecorator implements MiddlewareInterface
{
    public function __construct(
        private ContainerInterface $container,
        private MiddlewareFactory $factory,
        private string $middleware
    ) {}

    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        return $this->factory->prepare(
            $this->container->get(
                $this->middleware
            )
        )->process($request, $handler);
    }
}
