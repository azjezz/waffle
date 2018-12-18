<?hh // strict

namespace Waffle\Contract\SimpleCache;

use type Exception;

/**
 * Interface used for all types of exceptions thrown by the implementing library.
 */
interface CacheExceptionInterface
{
    require extends Exception;
}
