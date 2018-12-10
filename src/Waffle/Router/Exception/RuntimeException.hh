<?hh // strict

namespace Waffle\Router\Exception;

use type RuntimeException as ParentException;

class RuntimeException extends ParentException implements ExceptionInterface
{
}
