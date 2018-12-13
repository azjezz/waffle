<?hh // strict

namespace Waffle\Log\Exception;

use type LogicException as ParentException;

class LogicException extends ParentException implements ExceptionInterface
{
}
