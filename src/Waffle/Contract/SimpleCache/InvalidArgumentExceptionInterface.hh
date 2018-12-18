<?hh // strict

namespace Waffle\Contract\SimpleCache;

use type InvalidArgumentException;

/**
 * Exception interface for invalid cache arguments.
 *
 * When an invalid argument is passed it must throw an exception which implements
 * this interface
 */
interface InvalidArgumentExceptionInterface extends CacheExceptionInterface
{
    require extends InvalidArgumentException;
}
