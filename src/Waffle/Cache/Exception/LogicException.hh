<?hh // strict

namespace Waffle\Cache\Exception;

use type LogicException as ParentException;

class LogicException extends ParentException implements ExceptionInterface
{
}
