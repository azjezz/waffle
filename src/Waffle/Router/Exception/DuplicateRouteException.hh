<?hh // strict

namespace Waffle\Router\Exception;

use type DomainException;

class DuplicateRouteException extends DomainException implements ExceptionInterface
{
}
