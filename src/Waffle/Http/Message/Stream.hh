<?hh // strict

namespace Waffle\Http\Message;

use Waffle\Contract\Http\Message\StreamInterface;

use function is_string;
use function is_resource;
use function fopen;
use function fclose;
use function fwrite;
use function fstat;
use function ftell;
use function fseek;
use function feof;
use function fread;
use function stream_get_contents;
use function stream_get_meta_data;
use function array_key_exists;

use const SEEK_SET;

<<__ConsistentConstruct>>
class Stream implements StreamInterface
{
    protected ?resource $stream;

    protected bool $seekable;

    protected bool $readable;

    protected bool $writable;

    protected mixed $uri;

    protected ?int $size;

    private static Set<string> $readHash = Set {
            'r', 'w+', 'r+', 'x+', 'c+',
            'rb', 'w+b', 'r+b', 'x+b',
            'c+b', 'rt', 'w+t', 'r+t',
            'x+t', 'c+t', 'a+',
    };

    private static Set<string> $writeHash = Set {
            'w', 'w+', 'rw', 'r+', 'x+',
            'c+', 'wb', 'w+b', 'r+b',
            'x+b', 'c+b', 'w+t', 'r+t',
            'x+t', 'c+t', 'a', 'a+',
    };

    public function __construct(resource $body)
    {
        $this->stream = $body;
        $meta = stream_get_meta_data($this->stream);
        $this->seekable = $meta['seekable'] ?? false;
        $wrMode = $meta['mode'];
        $this->readable = self::$readHash->contains($wrMode);
        $this->writable = self::$writeHash->contains($wrMode);
        $this->uri = $this->getMetadata('uri');
    }

    public static function createFromString(string $body): StreamInterface 
    {
        $resource = fopen('php://temp', 'rw+');
        fwrite($resource, $body);
        return new self($resource);
    }

    public function __toString(): string
    {
        try {
            if ($this->isSeekable()) {
                $this->seek(0);
            }
            return $this->getContents();
        } catch (\Exception $e) {
            return '';
        }
    }

    public function close(): void
    {
        if (null !== $this->stream) {
            if (is_resource($this->stream)) {
                fclose($this->stream);
            }
            $this->detach();
        }
    }

    public function detach(): ?resource 
    {
        if (null === $this->stream) {
            return null;
        }

        $result = $this->stream;

        $this->stream = null;
        
        $this->size = $this->uri = null;
        $this->readable = $this->writable = $this->seekable = false;

        return $result;
    }

    public function getSize(): ?int
    {
        if (null !== $this->size) {
            return $this->size;
        }

        if (null === $this->stream) {
            return null;
        }

        $stats = new Map<string, mixed>(fstat($this->stream));

        if ($stats->contains('size')) {
            $this->size = (int) $stats->get('size');
            return $this->size;
        }

        return null;
    }

    public function tell(): int
    {
        if (null === $this->stream) {
            throw Exception\UntellableStreamException::dueToMissingResource();
        }

        $result = ftell($this->stream);

        if (false === $result) {
            throw Exception\UntellableStreamException::dueToPhpError();
        }

        return $result;
    }

    public function eof(): bool
    {
        return null === $this->stream || feof($this->stream);
    }

    public function isSeekable(): bool
    {
        return $this->seekable;
    }

    public function seek(int $offset,int $whence = \SEEK_SET): void
    {
        if (null === $this->stream) {
            throw Exception\UnseekableStreamException::dueToMissingResource();
        }

        if (!$this->seekable) {
            throw Exception\UnseekableStreamException::dueToConfiguration();
        }
        
        $retval = fseek($this->stream, $offset, $whence);
        
        if ($retval === -1) {
            throw Exception\UnseekableStreamException::dueToPhpError();
        }
    }

    public function rewind(): void
    {
        $this->seek(0);
    }

    public function isWritable(): bool
    {
        return $this->writable;
    }

    public function write(string $string): int
    {
        if (null === $this->stream) {
            throw Exception\UnwritableStreamException::dueToMissingResource();
        }

        if (!$this->writable) {
            throw Exception\UnwritableStreamException::dueToConfiguration();
        }

        $this->size = null;

        $result = fwrite($this->stream, $string);

        if (false === $result) {
            throw Exception\UnwritableStreamException::dueToPhpError();
        }

        return $result;
    }

    public function isReadable(): bool
    {
        return $this->readable;
    }

    public function read(int $length): string
    {
        if (null === $this->stream) {
            throw Exception\UnreadableStreamException::dueToMissingResource();
        }

        if (!$this->readable) {
            throw Exception\UnreadableStreamException::dueToConfiguration();
        }

        $result = fread($this->stream, $length);

        if (false === $result) {
            throw Exception\UnreadableStreamException::dueToPhpError();
        }

        return $result;
    }

    public function getContents(): string
    {
        if (null === $this->stream) {
            throw Exception\UnreadableStreamException::dueToMissingResource();
        }

        $contents = stream_get_contents($this->stream);

        if (false === $contents ) {
            throw Exception\UnreadableStreamException::dueToPhpError();
        }

        return $contents;
    }

    public function getMetadata(?string $key = null): mixed
    {
        if (null === $this->stream) {
            return null === $key ? null: [];
        }

        $meta = stream_get_meta_data($this->stream);

        if (null === $key) {
            return $meta;
        }

        return array_key_exists($key, $meta) ? $meta[$key]: null;
    }
}
