<?hh // strict

namespace Waffle\Log\Exception;

use type UnexpectedValueException as ParentException;

class UnexpectedValueException extends ParentException implements ExceptionInterface
{
}
