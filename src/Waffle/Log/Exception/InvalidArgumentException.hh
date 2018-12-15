<?hh // strict

namespace Waffle\Log\Exception;

use type InvalidArgumentException as ParentException;

class InvalidArgumentException extends ParentException implements ExceptionInterface
{
}
