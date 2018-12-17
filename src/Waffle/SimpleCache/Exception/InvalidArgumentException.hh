<?hh // strict

namespace Waffle\SimpleCache\Exception;

use type InvalidArgumentException as ParentException;
use type Waffle\Contract\SimpleCache\InvalidArgumentExceptionInterface;

class InvalidArgumentException extends ParentException implements ExceptionInterface, InvalidArgumentExceptionInterface
{
}
