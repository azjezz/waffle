<?hh // strict

namespace Waffle\Http\Message;

use Waffle\Contract\Http\Message\UriInterface;

use function parse_url;
use function preg_replace_callback;
use function rawurlencode;
use function sprintf;
use function strlen;
use function ltrim;
use function strtolower;

final class Uri implements UriInterface
{
    private static Map<string, int> $schemes = Map {
        'http' => 80, 
        'https' => 443
    };

    private static string $charUnreserved = 'a-zA-Z0-9_\-\.~';

    private static string $charSubDelims = '!\$&\'\(\)\*\+,;=';

    private string $scheme = '';

    private string $userInfo = '';

    private string $host = '';

    private ?int $port;

    private string $path = '';

    private string $query = '';

    private string $fragment = '';

    public function __construct(string $uri = '')
    {
        if ('' !== $uri) {
            $parts = parse_url($uri);

            if (false === $parts) {
                throw new Exception\InvalidArgumentException("Unable to parse URI: $uri");
            }

            $this->applyParts(new Map<string, arraykey>($parts));
        }
    }

    public function __toString(): string
    {
        return self::createUriString(
            $this->scheme, 
            $this->getAuthority(), 
            $this->path, 
            $this->query, 
            $this->fragment
        );
    }

    public function getScheme(): string
    {
        return $this->scheme;
    }

    public function getAuthority(): string
    {
        if ('' === $this->host) {
            return '';
        }

        $authority = $this->host;

        if ('' !== $this->userInfo) {
            $authority = $this->userInfo.'@'.$authority;
        }

        if (null !== $this->port) {
            $authority .= ':' . ((string) $this->port);
        }

        return $authority;
    }

    public function getUserInfo(): string
    {
        return $this->userInfo;
    }

    public function getHost(): string
    {
        return $this->host;
    }

    public function getPort(): ?int
    {
        return $this->port;
    }

    public function getPath(): string
    {
        return $this->path;
    }

    public function getQuery(): string
    {
        return $this->query;
    }

    public function getFragment(): string
    {
        return $this->fragment;
    }

    public function withScheme(string $scheme): this
    {
        $scheme = strtolower($scheme);

        if ($this->scheme === $scheme) {
            return $this;
        }

        $new = clone $this;
        $new->scheme = $scheme;
        $new->port = $new->filterPort($new->port);

        return $new;
    }

    public function withUserInfo(string $user, ?string $password = null): this
    {
        $info = $user;

        if (null !== $password && '' !== $password) {
            $info .= ':'.$password;
        }

        if ($this->userInfo === $info) {
            return $this;
        }

        $new = clone $this;
        $new->userInfo = $info;

        return $new;
    }

    public function withHost(string $host): this
    {
        $host = strtolower($host);

        if ($this->host === $host) {
            return $this;
        }

        $new = clone $this;
        $new->host = $host;

        return $new;
    }

    public function withPort(int $port): this
    {
        $port = $this->filterPort($port);

        if ($this->port === $port) {
            return $this;
        }

        $new = clone $this;
        $new->port = $port;

        return $new;
    }

    public function withPath(string $path): this
    {
        $path = $this->filterPath($path);

        if ($this->path === $path) {
            return $this;
        }

        $new = clone $this;
        $new->path = $path;

        return $new;
    }

    public function withQuery(string $query): this
    {
        $query = $this->filterQueryAndFragment($query);
        if ($this->query === $query) {
            return $this;
        }

        $new = clone $this;
        $new->query = $query;

        return $new;
    }

    public function withFragment(string $fragment): this
    {
        $fragment = $this->filterQueryAndFragment($fragment);

        if ($this->fragment === $fragment) {
            return $this;
        }

        $new = clone $this;
        $new->fragment = $fragment;

        return $new;
    }

    /**
     * Apply parse_url parts to a URI.
     */
    private function applyParts(Map<string, arraykey> $parts): void
    {
        $this->scheme = $parts->contains('schema') ? strtolower($parts->get('schema')): '';
        $this->host = $parts->contains('host') ? strtolower($parts->get('host')): '';
        $this->port = $parts->contains('port') ? $this->filterPort((int) $parts->get('port')): null;
        $this->path = $parts->contains('path') ? $this->filterPath((string) $parts->get('path')): '';
        $this->query = $parts->contains('query') ? $this->filterQueryAndFragment((string) $parts->get('query')): '';
        $this->fragment = $parts->contains('fragment') ? $this->filterQueryAndFragment((string) $parts->get('fragment')): '';
    
        if ($parts->contains('user')) {
            $this->userInfo = (string) $parts->get('user');
            if ($parts->contains('pass')) {
                $this->userInfo .= ':' . $parts->get('pass');
            }
        } else {
            $this->userInfo = '';
        }
    }

    /**
     * Create a URI string from its various parts.
     */
    private static function createUriString(string $scheme, string $authority, string $path, string $query, string $fragment): string
    {
        $uri = '';

        if ('' !== $scheme) {
            $uri .= $scheme.':';
        }

        if ('' !== $authority) {
            $uri .= '//'.$authority;
        }

        if (strlen($path) > 0) {
            if ('/' !== $path[0]) {
                if ('' !== $authority) {
                    // If the path is rootless and an authority is present, the path MUST be prefixed by "/"
                    $path = '/'.$path;
                }
            } elseif (strlen($path) > 1 && '/' === $path[1]) {
                if ('' === $authority) {
                    // If the path is starting with more than one "/" and no authority is present, the
                    // starting slashes MUST be reduced to one.
                    $path = '/'.ltrim($path, '/');
                }
            }

            $uri .= $path;
        }

        if ('' !== $query) {
            $uri .= '?'.$query;
        }

        if ('' !== $fragment) {
            $uri .= '#'.$fragment;
        }

        return $uri;
    }

    /**
     * Is a given port non-standard for the current scheme?
     */
    private static function isNonStandardPort(string $scheme, int $port): bool
    {
        return !self::$schemes->contains($scheme) || $port !== self::$schemes->get($scheme);
    }

    private function filterPort(?int $port): ?int
    {
        if (null === $port) {
            return null;
        }

        if (1 > $port || 0xffff < $port) {
            throw new Exception\InvalidArgumentException(
                sprintf('Invalid port: %d. Must be between 1 and 65535', $port)
            );
        }

        return self::isNonStandardPort($this->scheme, $port) ? $port: null;
    }

    private function filterPath(string $path): string
    {
        return preg_replace_callback(
            '/(?:[^'.self::$charUnreserved.self::$charSubDelims.'%:@\/]++|%(?![A-Fa-f0-9]{2}))/', 
            ($math) ==> rawurlencode($math[0]), 
            $path
        );
    }

    private function filterQueryAndFragment(string $str): string
    {
        return preg_replace_callback(
            '/(?:[^'.self::$charUnreserved.self::$charSubDelims.'%:@\/\?]++|%(?![A-Fa-f0-9]{2}))/', 
            ($math) ==> rawurlencode($math[0]), 
            $str
        );
    }
}
