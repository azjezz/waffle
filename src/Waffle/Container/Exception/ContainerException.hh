<?hh // strict

namespace Waffle\Container\Exception;

use type Waffle\Contract\Container\ContainerExceptionInterface;
use type RuntimeException;

class ContainerException extends RuntimeException implements ContainerExceptionInterface
{
}
