<?hh // strict

namespace Waffle\Container\Exception;

use type Waffle\Contract\Container\NotFoundExceptionInterface;
use type InvalidArgumentException;

class NotFoundException extends InvalidArgumentException implements NotFoundExceptionInterface
{
}
