<?hh // strict

namespace Waffle\Http\Server;

use namespace HH\Lib\C;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Container\ContainerInterface;
use type ReflectionFunctionAbstract;
use type ReflectionMethod;
use type ReflectionFunction;
use function is_callable;
use function is_object;
use function is_array;

class MiddlewareFactory
{
    public function __construct(
        protected ?ContainerInterface $container = null
    ) {}

    public function prepare(mixed $middleware): MiddlewareInterface
    {
        if ($middleware is MiddlewareInterface) {
            return $middleware;
        }

        if ($middleware is RequestHandlerInterface) {
            return new Middleware\RequestHandlerMiddleware($middleware);
        }

        if ($middleware instanceof Traversable || is_array($middleware)) {
            $pipe = new MiddlewarePipe();
            foreach ($middleware as $value) {
                $pipe->pipe(
                    $this->prepare($value)
                );
            }
            return $pipe;
        }

        if ($this->container is nonnull && $middleware is string && $this->container->has($middleware)) {
            return new Middleware\LazyMiddlewareDecorator($this->container, $this, $middleware);
        }

        if (is_callable($middleware, false)) {

            if (is_object($middleware)) {
                $reflection = new ReflectionMethod($middleware, '__invoke');
            } elseif (is_array($middleware) && C\count($middleware) === 2) {
                list($object, $method) = $middleware;
                $reflection = new ReflectionMethod($object, $method);
            } else {
                $reflection = new ReflectionFunction($middleware);
            }

            if ($this->isCallableMiddleware($reflection)) {
                /* HH_IGNORE_ERROR[4110] */
                return new Middleware\CallableMiddlewareDecorator($middleware);
            } elseif ($this->isCallableHandler($reflection)) {
                return $this->prepare(
                    /* HH_IGNORE_ERROR[4110] */
                    new RequestHandler\CallableRequestHandlerDecorator($middleware)
                );
            } elseif ($this->isDoublePassMiddleware($reflection)) {
                /* HH_IGNORE_ERROR[4110] */
                return new Middleware\DoublePassMiddlewareDecorator($middleware);
            }
        }

        throw Exception\InvalidMiddlewareException::forMiddleware($middleware);
    }

    private function isCallableMiddleware(ReflectionFunctionAbstract $reflection): bool
    {
        $parameters = $reflection->getParameters();

        if (C\count($parameters) !== 2) {
            return false;
        }

        $request = $parameters[0]->getType();
        $handler = $parameters[1]->getType();
        $return = $reflection->getReturnType();

        if (null === $request || null === $handler || null === $return) {
            return false;
        }

        if (
            ((string) $request) !== ServerRequestInterface::class ||
            ((string) $handler) !== RequestHandlerInterface::class ||
            ((string) $return)  !== ResponseInterface::class
        ) {
            return false;
        }

        return true;
    }

    private function isCallableHandler(ReflectionFunctionAbstract $reflection): bool
    {
        $parameters = $reflection->getParameters();

        if (C\count($parameters) !== 1) {
            return false;
        }

        $request = $parameters[0]->getType();
        $return = $reflection->getReturnType();

        if (null === $request || null === $return) {
            return false;
        }

        if (
            ((string) $request) !== ServerRequestInterface::class ||
            ((string) $return)  !== ResponseInterface::class
        ) {
            return false;
        }

        return true;
    }

    private function isDoublePassMiddleware(ReflectionFunctionAbstract $reflection): bool
    {
        $parameters = $reflection->getParameters();

        if (C\count($parameters) !== 3) {
            return false;
        }

        $request = $parameters[0]->getType();
        $response = $parameters[1]->getType();
        $handler = $parameters[2]->getType();
        $return = $reflection->getReturnType();

        if (null === $request || null === $response || null === $handler || null === $return) {
            return false;
        }

        if (
            ((string) $request) !== ServerRequestInterface::class ||
            ((string) $response) !== ResponseInterface::class ||
            ((string) $handler) !== RequestHandlerInterface::class ||
            ((string) $return)  !== ResponseInterface::class
        ) {
            return false;
        }

        return true;
    }

}
