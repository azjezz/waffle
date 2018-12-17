<?hh // strict

namespace Waffle\SimpleCache\Exception;

use namespace Waffle\SimpleCache;
use namespace Waffle\Contract\Cache;
use namespace HH\Lib\Str;
use type RuntimeException;

<<__ConsistentConstruct>>
class MissingDependencyException extends RuntimeException implements ExceptionInterface
{
    public static function forService(string $service): this
    {
        return new static(Str\format(
            '%s requires the service "%s" in order to build a %s instance; none found',
            SimpleCache\SimpleCacheServiceProvider::class,
            $service,
            SimpleCache\Cache::class
        ));
    }
}