<?hh // strict

namespace Waffle\Http\Message\Exception;

use RuntimeException;
use Exception;

class UploadedFileAlreadyMovedException extends RuntimeException implements ExceptionInterface
{
    public function __construct(
        string $message = 'Cannot retrieve stream after it has already moved',
        int $code = 0,
        ?Exception $previous = null
    ) {
        parent::__construct($message, $code, $previous);
    }
}
