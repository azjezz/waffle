<?hh // strict

namespace Waffle\Container\Exception;

use Waffle\Contract\Container\ContainerExceptionInterface;
use RuntimeException;

class ContainerException extends RuntimeException implements ContainerExceptionInterface
{
}