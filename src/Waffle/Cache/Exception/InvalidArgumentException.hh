<?hh // strict

namespace Waffle\Cache\Exception;

use type InvalidArgumentException as ParentException;
use type Waffle\Contract\Cache\InvalidArgumentExceptionInterface;

class InvalidArgumentException extends ParentException implements ExceptionInterface, InvalidArgumentExceptionInterface
{
}
