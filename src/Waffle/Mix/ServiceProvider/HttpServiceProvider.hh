<?hh // strict

namespace Waffle\Mix\ServiceProvider;

use namespace Waffle\Http;
use namespace Waffle\Contract\Cache;
use type Waffle\Contract\Http\Emitter\EmitterInterface;
use type Waffle\Contract\Http\Kernel\KernelInterface;
use type Waffle\Contract\Http\Message\ResponseFactoryInterface;
use type Waffle\Contract\Http\Message\RequestFactoryInterface;
use type Waffle\Contract\Http\Message\ServerRequestFactoryInterface;
use type Waffle\Contract\Http\Message\StreamFactoryInterface;
use type Waffle\Contract\Http\Message\UploadsFolderFactoryInterface;
use type Waffle\Contract\Http\Message\UploadedFileFactoryInterface;
use type Waffle\Contract\Http\Message\CookieFactoryInterface;
use type Waffle\Contract\Http\Message\UriFactoryInterface;
use type Waffle\Contract\Http\Server\MiddlewarePipeInterface;
use type Waffle\Contract\Http\Session\SessionInterface;
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use type Waffle\Container\Argument\RawArgument;

class HttpServiceProvider extends AbstractServiceProvider
{
    public function __construct(
        private Http\Session\SessionOptions $sessionOptions = shape()
    ) {
        parent::__construct();
    }

    protected vec<string> $provides = vec[
        EmitterInterface::class,
        KernelInterface::class,
        ResponseFactoryInterface::class,
        RequestFactoryInterface::class,
        ServerRequestFactoryInterface::class,
        StreamFactoryInterface::class,
        UploadsFolderFactoryInterface::class,
        UploadedFileFactoryInterface::class,
        CookieFactoryInterface::class,
        UriFactoryInterface::class,
        MiddlewarePipeInterface::class,
        Http\Server\MiddlewareFactory::class,
        Http\Session\SessionMiddleware::class,
        Http\Session\SessionOptions::class,
        Http\Session\Persistence\SessionPersistenceInterface::class,
        Http\Session\Persistence\NativeSessionPersistence::class,
        Http\Session\Persistence\CacheSessionPersistence::class,
    ];

    <<__Override>>
    public function register(): void
    {
        // Emitter
        $this->share(EmitterInterface::class, Http\Emitter\Emitter::class);
        // Kernel
        $this->share(KernelInterface::class, Http\Kernel\Kernel::class)
            ->addArgument(MiddlewarePipeInterface::class)
            ->addArgument(EmitterInterface::class);
        // Messages
        $this->share(ResponseFactoryInterface::class, Http\Message\Factory::class);
        $this->share(RequestFactoryInterface::class, Http\Message\Factory::class);
        $this->share(ServerRequestFactoryInterface::class, Http\Message\Factory::class);
        $this->share(StreamFactoryInterface::class, Http\Message\Factory::class);
        $this->share(UploadsFolderFactoryInterface::class, Http\Message\Factory::class);
        $this->share(UploadedFileFactoryInterface::class, Http\Message\Factory::class);
        $this->share(CookieFactoryInterface::class, Http\Message\Factory::class);
        $this->share(UriFactoryInterface::class, Http\Message\Factory::class);
        // Server
        $this->share(MiddlewarePipeInterface::class, Http\Server\MiddlewarePipe::class);
        // Session
        $this->share(Http\Server\MiddlewareFactory::class)
            ->addArgument(new RawArgument($this->getContainer()));
        $this->share(Http\Session\SessionMiddleware::class)
            ->addArgument(Http\Session\Persistence\SessionPersistenceInterface::class);

        $this->share(Http\Session\Persistence\SessionPersistenceInterface::class, () ==> {
            $persistence = $this->sessionOptions['persistence'] ?? Http\Session\Persistence\NativeSessionPersistence::class;
            switch ($persistence) {
                case 'cache':
                    return $this->getContainer()->get(Http\Session\Persistence\CacheSessionPersistence::class);
                case 'native':
                    return $this->getContainer()->get(Http\Session\Persistence\NativeSessionPersistence::class);
                default:
                    return $this->getContainer()->get($persistence);
                    break;
            }
        });

        $this->share(Http\Session\Persistence\NativeSessionPersistence::class)
            ->addArgument(Http\Session\SessionOptions::class);

        $this->share(Http\Session\Persistence\CacheSessionPersistence::class)
            ->addArgument(Cache\CacheItemPoolInterface::class)
            ->addArgument(Http\Session\SessionOptions::class);
    }
}
