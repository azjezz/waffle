<?hh // strict

namespace Waffle\Http\Message\Exception;

use RuntimeException;

use function sprintf;

<<__ConsistentConstruct>>
class UploadedFileErrorException extends RuntimeException implements ExceptionInterface
{
    public static function forUnmovableFile(): this
    {
        return new static('Error occurred while moving uploaded file');
    }

    public static function dueToStreamUploadError(): this
    {
        return new static('Cannot retrieve stream due to upload error');
    }

    public static function dueToUnwritablePath(): this
    {
        return new static('Unable to write to designated path');
    }

    public static function dueToUnwritableTarget(string $targetDirectory): this
    {
        return new static(
            sprintf(
                'The target directory `%s` does not exists or is not writable',
                $targetDirectory
            )
        );
    }
}
