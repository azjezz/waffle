<?hh // strict

namespace Waffle\Http;

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
use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use type Waffle\Container\Argument\RawArgument;

class HttpServiceProvider extends AbstractServiceProvider
{
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
        Server\MiddlewareFactory::class
    ];

    <<__Override>>
    public function register(): void
    {
        $this->share(EmitterInterface::class, Emitter\Emitter::class);
        $this->share(KernelInterface::class, Kernel\Kernel::class)
            ->addArgument(MiddlewarePipeInterface::class)
            ->addArgument(EmitterInterface::class);
        $this->share(ResponseFactoryInterface::class, Message\Factory::class);
        $this->share(RequestFactoryInterface::class, Message\Factory::class);
        $this->share(ServerRequestFactoryInterface::class, Message\Factory::class);
        $this->share(StreamFactoryInterface::class, Message\Factory::class);
        $this->share(UploadsFolderFactoryInterface::class, Message\Factory::class);
        $this->share(UploadedFileFactoryInterface::class, Message\Factory::class);
        $this->share(CookieFactoryInterface::class, Message\Factory::class);
        $this->share(UriFactoryInterface::class, Message\Factory::class);
        $this->share(MiddlewarePipeInterface::class, Server\MiddlewarePipe::class);
        $this->share(Server\MiddlewareFactory::class)
            ->addArgument(new RawArgument($this->getContainer()));
    }
}
