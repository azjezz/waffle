<?hh // strict

namespace Waffle\Json\Exception;

use type InvalidArgumentException;

class JsonEncodeException extends InvalidArgumentException implements ExceptionInterface
{
}