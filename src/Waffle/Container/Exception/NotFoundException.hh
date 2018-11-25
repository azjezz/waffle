<?hh // strict

namespace Waffle\Container\Exception;

use Waffle\Contract\Container\NotFoundExceptionInterface;
use InvalidArgumentException;

class NotFoundException extends InvalidArgumentException implements NotFoundExceptionInterface
{
}