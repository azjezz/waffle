<?hh // strict

namespace Waffle\Contract\Http\Message;

interface UploadedFileFactoryInterface
{
    /**
     * Create a new uploaded file.
     *
     * If a size is not provided it will be determined by checking the size of
     * the file.
     *
     * @param StreamInterface $stream Underlying stream representing the
     *     uploaded file content.
     * @param int $size in bytes
     * @param ?UploadeFileError $error PHP file upload error or null.
     * @param string $clientFilename Filename as provided by the client, if any.
     * @param string $clientMediaType Media type as provided by the client, if any.
     *
     * @return UploadedFileInterface
     *
     * @throws \InvalidArgumentException If the file resource is not readable.
     */
    public function createUploadedFile(
        StreamInterface $stream,
        ?int $size = null,
        ?UploadedFileError $error = null,
        ?string $clientFilename = null,
        ?string $clientMediaType = null
    ): UploadedFileInterface;
}