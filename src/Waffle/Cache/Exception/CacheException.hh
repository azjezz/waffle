<?hh // strict

namespace Waffle\Cache\Exception;

use type Waffle\Contract\Cache\CacheExceptionInterface;
use type Exception;

class CacheException extends Exception implements ExceptionInterface, CacheExceptionInterface
{
}
