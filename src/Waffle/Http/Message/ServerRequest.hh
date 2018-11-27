<?hh // strict

namespace Waffle\Http\Message;

use Waffle\Contract\Http\Message\ServerRequestInterface;
use Waffle\Contract\Http\Message\StreamInterface;
use Waffle\Contract\Http\Message\UploadedFileInterface;
use Waffle\Contract\Http\Message\UriInterface;

class ServerRequest extends Request implements ServerRequestInterface
{
    use MessageTrait;
    use RequestTrait;

    protected Map<string, mixed> $attributes = Map {};

    protected Map<string, string> $cookieParams = Map {};

    protected ?Map<string, mixed> $parsedBody = null;

    protected Map<string, mixed> $queryParams = Map {};

    protected Map<string, string> $serverParams = Map {};

    protected Map<string, UploadedFileInterface> $uploadedFiles = Map {};

    public function __construct(
        string $method, 
        UriInterface $uri, 
        Map<string, Set<string>> $headers = Map {}, 
        ?StreamInterface $body = null, 
        string $version = '1.1', 
        Map<string, string> $serverParams = Map {}
    ) {
        $this->method = $method;
        $this->uri = $uri;
        $this->serverParams = $serverParams;
        parent::__construct($method, $uri, $headers, $body, $version);
    }

    public function getServerParams(): Map<string, string>
    {
        return $this->serverParams;
    }

    public function getUploadedFiles(): Map<string, UploadedFileInterface>
    {
        return $this->uploadedFiles;
    }

    public function withUploadedFiles(Map<string, UploadedFileInterface> $uploadedFiles): this
    {
        $new = clone $this;
        $new->uploadedFiles = $uploadedFiles;

        return $new;
    }

    public function getCookieParams(): Map<string, string>
    {
        return $this->cookieParams;
    }

    public function withCookieParams(Map<string, string> $cookies): this
    {
        $new = clone $this;
        $new->cookieParams = $cookies;

        return $new;
    }

    public function getQueryParams(): Map<string, mixed>
    {
        return $this->queryParams;
    }

    public function withQueryParams(Map<string, mixed> $query): this
    {
        $new = clone $this;
        $new->queryParams = $query;

        return $new;
    }

    public function getParsedBody(): ?Map<string, mixed>
    {
        return $this->parsedBody;
    }

    public function withParsedBody(?Map<string, mixed> $parsedBody): this
    {
        $new = clone $this;
        $new->parsedBody = $parsedBody;

        return $new;
    }

    public function getAttributes(): Map<string, mixed>
    {
        return $this->attributes;
    }

    public function getAttribute(string $attribute, mixed $default = null): mixed
    {
        return $this->attributes->get($attribute) ?? $default;
    }

    public function withAttribute(string $attribute, mixed $value): this
    {
        $new = clone $this;
        $new->attributes->set($attribute, $value);
        return $new;
    }

    public function withoutAttribute(string $attribute): this
    {
        if (!$this->attributes->contains($attribute)) {
            return $this;
        }

        $new = clone $this;
        $new->attributes->remove($attribute);

        return $new;
    }
}
