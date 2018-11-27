<?hh // strict

namespace Waffle\Http\Message\Exception;

use InvalidArgumentException as ParentException;

class InvalidArgumentException extends ParentException implements ExceptionInterface
{
}