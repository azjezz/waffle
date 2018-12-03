<?hh // strict

namespace Waffle\Http\ServerApi\Exception;

use type InvalidArgumentException as ParentException;

class InvalidArgumentException extends ParentException implements ExceptionInterface
{
}
