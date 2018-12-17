<?hh // strict

namespace Waffle\SimpleCache\Exception;

use type Waffle\Contract\SimpleCache\CacheExceptionInterface;
use type Exception;

class CacheException extends Exception implements ExceptionInterface, CacheExceptionInterface
{
}
