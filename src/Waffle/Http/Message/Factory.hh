<?hh // strict

namespace Waffle\Http\Message;

use namespace Waffle\Contract\Http\Message;
use function fopen;

class Factory implements
    Message\ResponseFactoryInterface,
    Message\RequestFactoryInterface,
    Message\ServerRequestFactoryInterface,
    Message\StreamFactoryInterface,
    Message\UploadsFolderFactoryInterface,
    Message\UploadedFileFactoryInterface,
    Message\CookieFactoryInterface,
    Message\UriFactoryInterface
{
    public function createResponse(int $code = 200, string $reasonPhrase = ''): Message\ResponseInterface
    {
        return (new Response())
            ->withStatus($code, $reasonPhrase);
    }

    public function createRequest(string $method, Message\UriInterface $uri): Message\RequestInterface
    {
        return new Request($method, $uri);
    }

    public function createServerRequest(string $method, Message\UriInterface $uri, Map<string, string> $serverParams = Map {}): Message\ServerRequestInterface
    {
        return new ServerRequest($method, $uri, Map {}, null, '1.1', $serverParams);
    }

    public function createStream(string $content = ''): Message\StreamInterface
    {
        return Functional\create_stream_from_string($content);
    }

    public function createStreamFromFile(string $filename, string $mode = 'r'): Message\StreamInterface
    {
        return new Stream(fopen($filename, $mode));
    }

    public function createStreamFromResource(resource $resource): Message\StreamInterface
    {
        return new Stream($resource);
    }

    public function createUploadsFolder(): Message\UploadsFolderInterface
    {
        return new UploadsFolder();
    }

    public function createUploadedFile(
        Message\StreamInterface $stream,
        ?int $size = null,
        Message\UploadedFileError $error = Message\UploadedFileError::ERROR_OK,
        ?string $clientFilename = null,
        ?string $clientMediaType = null
    ): Message\UploadedFileInterface
    {
        return new UploadedFile($stream, $size, $error, $clientFilename, $clientMediaType);
    }

    public function createCookie(string $value): Message\CookieInterface
    {
        return new Cookie($value);
    }

    public function createUri(string $uri = ''): Message\UriInterface
    {
        return new Uri($uri);
    }
}