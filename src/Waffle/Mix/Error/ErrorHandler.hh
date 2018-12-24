<?hh // strict

namespace Waffle\Mix\Error;

use namespace Waffle\Http\Message\Response;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Mix\Environment;
use type Throwable;

class ErrorHandler implements ErrorHandlerInterface
{
    public function __construct(
        protected Environment $env
    ) {}

    /**
     * Handle the error and return a response instance.
     *
     * This ErrorHandler returns a JsonResponse with formatted exception data following
     * the JSend specifications in debug-mode, or an EmptyResponse otherwise.
     *
     * @link https://labs.omniti.com/labs/jsend
     */
    public function handle(Throwable $error, ServerRequestInterface $_request): ResponseInterface
    {
        $code = (int) $error->getCode();
        if ($code < 400 || $code > 600) {
            $code = 500;
        }

        if (!$this->env->debug()) {
            return new Response\EmptyResponse($code);
        }

        return new Response\JsonResponse(dict[
            'status' => 'error',
            'message' => $error->getMessage(),
            'code' => $code,
            'data' => dict[
                'file' => $error->getFile(),
                'line' => $error->getLine(),
                'trace' => $error->getTrace()
            ]
        ], $code);
    }
}
