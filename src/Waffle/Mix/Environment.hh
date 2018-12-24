<?hh // strict

namespace Waffle\Mix;

<<__ConsistentConstruct>>
class Environment
{
    const string ENV_DEV    = 'development';
    const string ENV_PROD   = 'production';
    const string ENV_TEST   = 'test';

    public function __construct(
        protected string $environment,
        protected bool $debug
    ) {}

    public static function development(bool $debug = true): this
    {
        return new static(static::ENV_DEV, $debug);
    }

    public static function production(bool $debug = false): this
    {
        return new static(static::ENV_PROD, $debug);
    }

    public static function test(): this
    {
        return new static(static::ENV_TEST, true);
    }

    public function debug(): bool
    {
        return $this->debug;
    }

    public function env(): string
    {
        return $this->environment;
    }
}
