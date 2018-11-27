<?hh // strict

namespace Waffle\Http\Message;

use Waffle\Contract\Http\Message\RequestInterface;
use Waffle\Contract\Http\Message\StreamInterface;
use Waffle\Contract\Http\Message\UriInterface;

class Request implements RequestInterface
{
    use MessageTrait;
    use RequestTrait;

    public function __construct(
        string $method, 
        UriInterface $uri, 
        Map<string, Set<string>> $headers = Map {}, 
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

    public function __clone(): void 
    {
        $this->headers = clone $this->headers;
        $this->headerNames = clone $this->headerNames;
        $this->stream = null === $this->stream ? null : (clone $this->stream);
        $this->uri = clone $this->uri;
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

        if ($this->headerNames->contains('host')) {
            $header = $this->headerNames->get('host') ?? 'Host';            
        } else {
            $header = 'Host';
            $this->headerNames->set('host', 'Host');
        }

        $this->headers->add(Pair { 
            $header, Set { $host } 
        });
    }
}
