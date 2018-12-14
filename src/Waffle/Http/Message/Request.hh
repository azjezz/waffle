<?hh // strict

namespace Waffle\Http\Message;

use namespace HH\Lib\C;
use namespace Waffle\Http\Message\__Private;
use type Waffle\Contract\Http\Message\RequestInterface;
use type Waffle\Contract\Http\Message\StreamInterface;
use type Waffle\Contract\Http\Message\UriInterface;

class Request implements RequestInterface
{
    use MessageTrait;
    use RequestTrait;

    public function __construct(
        string $method,
        UriInterface $uri,
        dict<string, vec<string>> $headers = dict[],
        ?StreamInterface $body = null,
        string $version = '1.1'
    ) {
        $this->method = $method;
        $this->uri = $uri;
        $this->setHeaders($headers);
        $this->protocol = $version;

        if (!$this->hasHeader('Host')) {
            $this->updateHostFromUri();
        }

        if (null !== $body) {
            $this->stream = $body;
        }
    }

    protected function updateHostFromUri(): void
    {
        if ('' === $host = $this->uri->getHost()) {
            return;
        }

        $port = $this->uri->getPort();

        if (null !== $port) {
            $host .= ':' . ((string) $port);
        }

        if (C\contains_key($this->headerNames, 'host')) {
            $header = $this->headerNames['host'];
        } else {
            $header = 'Host';
            $this->headerNames['host'] = 'Host';
        }

        $this->headers = __Private\dict_union(
            dict[ $header => vec[$host]],
            $this->headers,
        );
    }

    public function __clone(): void
    {
        $this->messageClone();
    }
}
