<?hh // strict

namespace Waffle\Mix\Error;

use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Throwable;

interface ErrorHandlerInterface
{
    /**
     * Handle the error and return a response instance.
     */
    public function handle(Throwable $error, ServerRequestInterface $request): ResponseInterface;
}
