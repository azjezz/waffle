<?hh // strict

namespace Waffle\Mix\Session;

use type Waffle\Container\ServiceProvider\AbstractServiceProvider;
use type Waffle\Container\Argument\RawArgument;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\Http\Session\SessionPersistenceInterface;
use type Waffle\Http\Session\SessionCookieOptions;

class SessionPersistenceServiceProvider extends AbstractServiceProvider
{
    protected vec<string> $provides = vec[
        SessionPersistenceInterface::class
    ];

    <<__Override>>
    public function __construct(private SessionOptions $options)
    {
        parent::__construct();
    }

    <<__Override>>
    public function register(): void
    {
        $this->share(SessionPersistenceInterface::class, SessionPersistence::class)
            ->addArgument(CacheItemPoolInterface::class)
            ->addArgument(new RawArgument($this->options));
    }
}