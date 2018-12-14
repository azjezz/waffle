<?hh // strict

namespace Waffle\Http\Message;

use namespace HH\Lib\C;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\StreamInterface;
use type Waffle\Contract\Http\Message\UriInterface;
use type Waffle\Contract\Http\Message\UploadsFolderInterface;

class ServerRequest extends Request implements ServerRequestInterface
{
    protected dict<string, mixed> $attributes = dict[];

    protected dict<string, string> $cookieParams = dict[];

    protected ?dict<string, mixed> $parsedBody = null;

    protected dict<string, mixed> $queryParams = dict[];

    protected ?UploadsFolderInterface $uploadsFolder;

    public function __construct(
        string $method,
        UriInterface $uri,
        dict<string, vec<string>> $headers = dict[],
        ?StreamInterface $body = null,
        string $version = '1.1',
        protected dict<string, mixed> $serverParams = dict[]
    ) {
        $this->method = $method;
        $this->uri = $uri;
        parent::__construct($method, $uri, $headers, $body, $version);
    }

    <<__Override>>
    public function __clone(): void
    {
        parent::__clone();
        $this->uploadsFolder = null === $this->uploadsFolder ? null : clone $this->uploadsFolder;
    }

    /**
     * Create a new Http Server Request Message from the global variables.
     *
     * @see Factory->createServerRequestFromGlobals()
     */
    public static function capture(): ServerRequestInterface
    {
        return (new Factory())->createServerRequestFromGlobals();
    }

    public function getServerParams(): dict<string, mixed>
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

    public function getCookieParams(): dict<string, string>
    {
        return $this->cookieParams;
    }

    public function withCookieParams(dict<string, string> $cookies): this
    {
        $new = clone $this;
        $new->cookieParams = $cookies;

        return $new;
    }

    public function getQueryParams(): dict<string, mixed>
    {
        return $this->queryParams;
    }

    public function withQueryParams(dict<string, mixed> $query): this
    {
        $new = clone $this;
        $new->queryParams = $query;

        return $new;
    }

    public function getParsedBody(): ?dict<string, mixed>
    {
        return $this->parsedBody;
    }

    public function withParsedBody(?dict<string, mixed> $parsedBody): this
    {
        $new = clone $this;
        $new->parsedBody = $parsedBody;

        return $new;
    }

    public function getAttributes(): dict<string, mixed>
    {
        return $this->attributes;
    }

    public function getAttribute(string $attribute, mixed $default = null): mixed
    {
        if (C\contains_key($this->attributes, $attribute)) {
            return $this->attributes[$attribute];
        }
        return $default;
    }

    public function withAttribute(string $attribute, mixed $value): this
    {
        $new = clone $this;
        $new->attributes[$attribute] = $value;
        return $new;
    }

    public function withoutAttribute(string $attribute): this
    {
        if (!C\contains_key($this->attributes,$attribute)) {
            return $this;
        }

        $new = clone $this;
        unset($new->attributes[$attribute]);

        return $new;
    }
}
