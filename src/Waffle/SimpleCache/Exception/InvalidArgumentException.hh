<?hh // strict

namespace Waffle\SimpleCache\Exception;

use type InvalidArgumentException as ParentException;

class InvalidArgumentException extends ParentException implements ExceptionInterface
{
}
