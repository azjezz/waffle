<?hh // strict

namespace Waffle\Contract\Http\Server;

use type Waffle\Contract\Service\ResetInterface;

/**
 * Pipe middleware like unix pipes.
 *
 * This class implements a pipeline of middleware, which can be attached using
 * the `pipe()` method, and is itself middleware.
 *
 * Inspired by Sencha Connect.
 *
 * @see https://github.com/senchalabs/connect
 */
interface MiddlewarePipeInterface extends MiddlewareInterface, RequestHandlerInterface, ResetInterface
{
    public function pipe(MiddlewareInterface $middleware): void;
}
