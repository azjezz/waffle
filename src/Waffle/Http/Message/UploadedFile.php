<?hh // strict

namespace Waffle\Http\Message;

use Waffle\Contract\Http\Message\StreamInterface;
use Waffle\Contract\Http\Message\UploadedFileInterface;
use Waffle\Contract\Http\Message\UploadedFileError;

use function fopen;
use function strlen;

class UploadedFile implements UploadedFileInterface
{
    private bool $moved = false;


    /**
     * @param StreamInterface|string|resource $streamOrFile
     * @param int                             $size
     * @param int                             $errorStatus
     * @param string|null                     $clientFilename
     * @param string|null                     $clientMediaType
     */
    public function __construct(
        private StreamInterface $stream,
        private ?int $size,
        private UploadedFileError $error, 
        private ?string $clientFilename = null, 
        private ?string $clientMediaType = null
    ) {}

    /**
     * @return bool return true if there is no upload error
     */
    private function isOk(): bool
    {
        return UploadedFileError::ERROR_OK === $this->error;
    }

    /**
     * @throws Exception\ExceptionInterface if is moved or not ok
     */
    private function validateActive(): void
    {
        if (false === $this->isOk()) {
            throw Exception\UploadedFileErrorException::dueToStreamUploadError();
        }

        if ($this->moved) {
            throw new Exception\UploadedFileAlreadyMovedException();
        }
    }

    public function getStream(): StreamInterface
    {
        $this->validateActive();
        return $this->stream;
    }

    public function moveTo(string $targetPath): void
    {
        $this->validateActive();

        if ('' === $targetPath) {
            throw new Exception\InvalidArgumentException(
                'Invalid path provided for move operation; must be a non-empty string'
            );
        }

        $stream = $this->getStream();
        if ($stream->isSeekable()) {
            $stream->rewind();
        }

        $handle = fopen($targetPath, 'w');

        if (false === $handle) {
            throw Exception\UploadedFileErrorException::dueToUnwritablePath();
        }

        $this->copyToStream($stream, new Stream($handle));
        $this->moved = true;
    }

    public function getSize(): ?int
    {
        return $this->size;
    }

    public function getError(): UploadedFileError
    {
        return $this->error;
    }

    public function getClientFilename(): ?string
    {
        return $this->clientFilename;
    }

    public function getClientMediaType(): ?string
    {
        return $this->clientMediaType;
    }

    /**
     * Copy the contents of a stream into another stream until the given number
     * of bytes have been read.
     *
     * @param StreamInterface $source Stream to read from
     * @param StreamInterface $dest   Stream to write to
     * @param int             $maxLen Maximum number of bytes to read. Pass -1
     *                                to read the entire stream
     *
     * @throws \RuntimeException on error
     */
    private function copyToStream(StreamInterface $source, StreamInterface $dest,int $maxLen = -1): void
    {
        if ($maxLen === -1) {
            while (!$source->eof()) {
                if (!$dest->write($source->read(1048576))) {
                    break;
                }
            }

            return;
        }

        $bytes = 0;
        while (!$source->eof()) {
            $buf = $source->read($maxLen - $bytes);
            if (!($len = strlen($buf))) {
                break;
            }
            $bytes += $len;
            $dest->write($buf);
            if ($bytes === $maxLen) {
                break;
            }
        }
    }
}
