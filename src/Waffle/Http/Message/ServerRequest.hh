<?hh // strict

namespace Waffle\Http\Message;

use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\StreamInterface;
use type Waffle\Contract\Http\Message\UriInterface;
use type Waffle\Contract\Http\Message\UploadsFolderInterface;

class ServerRequest extends Request implements ServerRequestInterface
{
    protected Map<string, mixed> $attributes = Map {};

    protected Map<string, string> $cookieParams = Map {};

    protected ?Map<string, mixed> $parsedBody = null;

    protected Map<string, mixed> $queryParams = Map {};

    protected ?UploadsFolderInterface $uploadsFolder;

    public function __construct(
        string $method,
        UriInterface $uri,
        Map<string, Set<string>> $headers = Map {},
        ?StreamInterface $body = null,
        string $version = '1.1',
        protected Map<string, mixed> $serverParams = Map {}
    ) {
        $this->method = $method;
        $this->uri = $uri;
        parent::__construct($method, $uri, $headers, $body, $version);
    }

    <<__Override>>
    public function __clone(): void
    {
        parent::__clone();
        $this->attributes = clone $this->attributes;
        $this->cookieParams = clone $this->cookieParams;
        $this->parsedBody = null === $this->parsedBody ? null : clone $this->parsedBody;
        $this->queryParams = clone $this->queryParams;
        $this->serverParams = clone $this->serverParams;
        $this->uploadsFolder = null === $this->uploadsFolder ? null : clone $this->uploadsFolder;
    }

    public function getServerParams(): Map<string, mixed>
    {
        return $this->serverParams;
    }

    public function getUploadsFolder(): ?UploadsFolderInterface
    {
        return $this->uploadsFolder;
    }

    public function withUploadsFolder(UploadsFolderInterface $uploadsFolder): this
    {
        $new = clone $this;
        $new->uploadsFolder = $uploadsFolder;

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
        if ($this->attributes->contains($attribute)) {
            return $this->attributes->at($attribute);
        }
        return $default;
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
