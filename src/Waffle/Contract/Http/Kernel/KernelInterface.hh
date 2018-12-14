<?hh // strict

namespace Waffle\Contract\Http\Kernel;

use type Waffle\Contract\Http\Server\MiddlewarePipeInterface;
use type Waffle\Contract\Http\Emitter\EmitterInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

interface KernelInterface extends MiddlewarePipeInterface, EmitterInterface
{
    /**
     * Perform any final actions for the request lifecycle.
     */
    public function terminate(ServerRequestInterface $request, ResponseInterface $response): void;
}
