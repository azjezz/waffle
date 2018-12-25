<?hh // strict

namespace Waffle\Mix;

use namespace HH\Lib\C;
use namespace Waffle;
use type Waffle\Contract\Config\ConfigurationInterface;
use type Waffle\Contract\Event\EventDispatcherInterface;
use type Waffle\Contract\Event\EventSubscriberInterface;
use type Waffle\Contract\Event\EventInterface;
use type Waffle\Contract\Event\EventListener;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Emitter\EmitterInterface;
use type Waffle\Contract\Http\Kernel\KernelInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Server\MiddlewareInterface;
use type Waffle\Contract\Service\ResetInterface;
use type Waffle\Container\ServiceProvider\ServiceProviderInterface;
use type Waffle\Container\Container;
use type Waffle\Config\Configuration;
use type Waffle\Router\RouterInterface;
use type Waffle\Router\RouteCollector;
use type Waffle\Http\Message\ServerRequest;
use type Waffle\Http\Server\MiddlewareFactory;
use type Waffle\Router\Route;
use type Throwable;

class Application implements MiddlewareInterface, RequestHandlerInterface, EmitterInterface, ResetInterface
{
    protected KernelInterface $kernel;
    protected RouterInterface $router;
    protected RouteCollector $collector;
    protected EventDispatcherInterface $events;
    protected MiddlewareFactory $middlewares;
    private vec<classname<Recipe\Recipe>> $recipes = vec[];

    public function __construct(
        protected Environment $environment,
        protected ConfigurationInterface $configuration,
        protected Container $container = new Container(),
    ) {
        $container->share('config', () ==> $configuration);
        $container->share('env', () ==> $environment);
        
        $container->share(ConfigurationInterface::class, () ==> $configuration);
        $container->share(Environment::class, () ==> $environment);
        $container->share(Error\ErrorHandlerInterface::class, Error\ErrorHandler::class)
            ->addArgument(Environment::class);
        $container->share(Middleware\ErrorMiddleware::class)
            ->addArgument(Error\ErrorHandlerInterface::class);
        $container->share(Handler\NotFoundHandler::class);

        $container->addServiceProvider(new Waffle\Http\HttpServiceProvider());
        $container->addServiceProvider(new Waffle\Router\RouterServiceProvider());
        $container->addServiceProvider(new Waffle\Event\EventServiceProvider());

        $this->middlewares  = new MiddlewareFactory($container);
        $this->kernel       = $container->get(KernelInterface::class) as KernelInterface;
        $this->router       = $container->get(RouterInterface::class) as RouterInterface;
        $this->collector    = $container->get(RouteCollector::class) as RouteCollector;
        $this->events       = $container->get(EventDispatcherInterface::class) as EventDispatcherInterface;

        $this->use(Recipe\BasicRecipe::class);
    }

    public function use(classname<Recipe\Recipe> $recipe): void
    {
        $this->recipes[] = $recipe;
        $recipe = new $recipe($this, $this->container) as Recipe\Recipe;
        $recipe->mix();
    }

    public function uses(classname<Recipe\Recipe> $recipe): bool
    {
        return C\contains($this->recipes, $recipe);
    }

    public function on(classname<EventInterface> $event, EventListener $listener, int $priority = 0): void
    {
        $this->events->on($event, $listener, $priority);
    }

    /**
     * Add a service provider.
     */
    public function register(ServiceProviderInterface $service): void
    {
        $event = $this->events->dispatch(
            new Event\RegisterEvent($service)
        ) as Event\RegisterEvent;

        $this->container->addServiceProvider($event->service);
    }

    /**
     * Register an event subscriber with the dispatcher.
     */
    public function subscribe(EventSubscriberInterface $subscriber): void
    {
        $event = $this->events->dispatch(
            new Event\SubscribeEvent($subscriber)
        ) as Event\SubscribeEvent;

        $this->events->subscribe($event->subscriber);
    }

    /*
     * Pipe middleware like unix pipes.
     */
    public function pipe(mixed $middleware, int $priority = 0): void
    {
        $middleware = $this->middlewares->prepare($middleware);

        $event = $this->events->dispatch(
            new Event\PipeEvent($middleware, $priority)
        ) as Event\PipeEvent;

        $this->kernel->pipe($event->middleware, $event->priority);
    }

    public function route(string $path, mixed $middleware, ?vec<string> $methods, ?string $name = null): Route
    {
        $middleware = $this->middlewares->prepare($middleware);
        return $this->collector->route($path, $middleware, $methods, $name);
    }

    public function get(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['GET'], $name);
    }

    public function post(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['POST'], $name);
    }

    public function put(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['PUT'], $name);
    }

    public function patch(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['PATCH'], $name);
    }

    public function delete(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, vec['DELETE'], $name);
    }

    public function any(string $path, mixed $middleware, ?string $name = null): Route
    {
        return $this->route($path, $middleware, null, $name);
    }

    public function fallback(mixed $middleware): void
    {
        $this->pipe($middleware, -0x9950);
    }

    /**
     * Process an incoming server request and return a response, optionally delegating
     * response creation to a handler.
     */
    public function process(ServerRequestInterface $request, RequestHandlerInterface $handler): ResponseInterface
    {
        $event = $this->events->dispatch(
            new Event\ProcessEvent($request, $handler)
        ) as Event\ProcessEvent;

        return $this->kernel->process($event->request, $event->handler);
    }

    /**
     * Handle the request and return a response.
     */
    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        $event = $this->events->dispatch(
            new Event\HandleEvent($request)
        ) as Event\HandleEvent;

        return $this->kernel->handle($event->request);
    }

    /**
     * Emit a response.
     *
     * Emits a response, including status line, headers, and the message body,
     * according to the environment.
     */
    public function emit(ResponseInterface $response): bool
    {
        $event = $this->events->dispatch(
            new Event\EmitEvent($response)
        ) as Event\EmitEvent;

        return $this->kernel->emit($event->response);
    }

    /**
     * Perform any final actions for the request lifecycle.
     */
    public function terminate(ServerRequestInterface $request, ResponseInterface $response): void
    {
        $event = $this->events->dispatch(
            new Event\TerminateEvent($request, $response)
        ) as Event\TerminateEvent;

        $this->kernel->terminate($event->request, $event->response);
    }

    public function run(): void
    {
        try {
            $request = ServerRequest::capture();
            $response = $this->handle($request);
            $this->emit($response);
            $this->terminate($request, $response);
        } catch(Throwable $e) {
            $event = $this->events->dispatch(
                new Event\ErrorEvent($e)
            ) as Event\ErrorEvent;

            if (!$event->handled) {
                throw $e;
            }
        }
    }

    /**
     * Reset the application to its initial status.
     */
    public function reset(): void
    {
        $this->container->reset();
        $this->middlewares  = new MiddlewareFactory($this->container);
        $this->kernel       = $this->container->get(KernelInterface::class, true) as KernelInterface;
        $this->router       = $this->container->get(RouterInterface::class, true) as RouterInterface;
        $this->collector    = $this->container->get(RouteCollector::class, true) as RouteCollector;
        $this->events       = $this->container->get(EventDispatcherInterface::class, true) as EventDispatcherInterface;
    }
}
