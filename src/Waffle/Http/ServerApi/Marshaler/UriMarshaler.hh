<?hh // strict

namespace Waffle\Http\ServerApi\Marshaler;

use namespace HH\Lib\Str;
use namespace Waffle\Http\ServerApi\Exception;
use type Waffle\Contract\Http\Message\UriInterface;
use type Waffle\Http\Message\Uri;
use function explode;
use function is_array;
use function preg_match;
use function preg_replace;
use function gettype;

class UriMarshaler
{
    public function marshal(Map<string, mixed> $server, Map<string, Set<string>> $headers): UriInterface
    {
        $uri = new Uri('');
        // URI scheme
        $scheme = 'http';
        if ($server->contains('HTTPS')) {
            $https = $this->marshalHttpsValue($server->at('HTTPS'));
        } elseif ($server->contains('https')) {
            $https = $this->marshalHttpsValue($server->at('https'));
        } else {
            $https = false;
        }

        if ($https || Str\lowercase((string) $this->getHeadersFromMap('x-forwarded-proto', $headers, '')) === 'https') {
            $scheme = 'https';
        }

        $uri = $uri->withScheme($scheme);

        // Set the host
        $hostAndPort = $this->marshalHostAndPort($headers, $server);
        $host = $hostAndPort['host'];
        $port = $hostAndPort['port'];

        if ($host !== '') {
            $uri = $uri->withHost($host);
            if (null !== $port) {
                $uri = $uri->withPort($port);
            }
        }

        $path = $this->marshalRequestPath($server);
        $path = explode('?', $path, 2)[0];
        $query = '';
        if ($server->contains('QUERY_STRING')) {
            $query = Str\trim_left((string) $server->at('QUERY_STRING') ?? '', '?');
        }
        // URI fragment
        $fragment = '';
        if (Str\search($path, '#') !== null) {
            list($path, $fragment) = explode('#', $path, 2);
        }
        return $uri
            ->withPath($path)
            ->withFragment($fragment)
            ->withQuery($query);
    }

    private function marshalIpv6HostAndPort(Map<string, mixed> $server, string $host, ?int $port): shape('host' => string, 'port' => ?int,...)
    {
        $host = '[' . ((string) $server->get('SERVER_ADDR') ?? '') . ']';
        $port = $port ?? 80;
        if ($port . ']' === Str\slice($host, ((int) Str\search_last($host, ':')) + 1)) {
            // The last digit of the IPv6-Address has been taken as port
            // Unset the port so the default port can be used
            $port = null;
        }

        return shape(
            'host' => $host,
            'port' => $port
        );
    }

    /**
     * From Symfony's Symfony\Component\HttpFoundation\Request class
     * @see https://github.com/symfony/symfony/blob/master/src/Symfony/Component/HttpFoundation/Request.php#L1149-L1161
     *
     * @copyright Copyright (c) 2004-2018 Fabien Potencier <fabien@symfony.com>
     * @license   https://github.com/symfony/symfony/blob/master/LICENSE MIT License
     */
    private function marshalHostFromHeader(mixed $host): string
    {
        if (is_array($host)) {
            $host = Str\join($host, ', ');
        }

        // trim and remove port number from host
        // host is lowercase as per RFC 952/2181
        $host = Str\lowercase(preg_replace('/:\d+$/', '', Str\trim((string) $host)));

        // as the host can come from the user (HTTP_HOST and depending on the configuration, SERVER_NAME too can come from the user)
        // check that it does not contain forbidden characters (see RFC 952 and RFC 2181)
        // use preg_replace() instead of preg_match() to prevent DoS attacks with long host names
        if ($host !== '' && '' !== preg_replace('/(?:^\[)?[a-zA-Z0-9-:\]_]+\.?/', '', $host)) {
            return '';
        }

        return $host;
    }

    /**
     * From Symfony's Symfony\Component\HttpFoundation\Request class
     * @see https://github.com/symfony/symfony/blob/master/src/Symfony/Component/HttpFoundation/Request.php#L910-L918
     *
     * @copyright Copyright (c) 2004-2018 Fabien Potencier <fabien@symfony.com>
     * @license   https://github.com/symfony/symfony/blob/master/LICENSE MIT License
     */
    private function marshalPortFromHeader(mixed $host): ?int
    {
        $host = (string) $host;

        if ('[' === Str\slice($host, 0, 1)) {
            $pos = Str\search($host, ':', (int) Str\search_last($host, ']'));
        } else {
            $pos = Str\search_last($host, ':');
        }

        if (null !== $pos) {
            return (int) Str\slice($host, $pos + 1);
        }

        return null;
    }

    /**
     * Marshal the host and port from HTTP headers and/or the PHP environment.
     *
     * @return shape('host' => string, 'port' => ?int,...) shape of two items, host and port, in that order.
     */
    private function marshalHostAndPort(Map<string, Set<string>> $headers, Map<string, mixed> $server): shape('host' => string, 'port' => ?int,...)
    {
        static $defaults = shape('host' => '', 'port' => null);

        $header = $this->getHeadersFromMap('host', $headers);
        if (null !== $header) {
            return shape(
                'host' => $this->marshalHostFromHeader($header),
                'port' => $this->marshalPortFromHeader($header)
            );
        }

        if (!$server->contains('SERVER_NAME')) {
            return $defaults;
        }

        $host = (string) $server->get('SERVER_NAME') ?? '';
        $port = $server->contains('SERVER_PORT') ? (int) $server->get('SERVER_PORT') : null;

        if (! $server->contains('SERVER_ADDR') || ! preg_match('/^\[[0-9a-fA-F\:]+\]$/', $host)) {
            return shape('host' => $host, 'port' => $port);
        }

        return $this->marshalIpv6HostAndPort($server, $host, $port);
    }

    /**
     * Detect the path for the request
     *
     * Looks at a variety of criteria in order to attempt to autodetect the base
     * request path, including:
     *
     * - IIS7 UrlRewrite environment
     * - REQUEST_URI
     * - ORIG_PATH_INFO
     *
     * From ZF2's Zend\Http\PhpEnvironment\Request class
     * @copyright Copyright (c) 2005-2015 Zend Technologies USA Inc. (http://www.zend.com)
     * @license   http://framework.zend.com/license/new-bsd New BSD License
     */
    private function marshalRequestPath(Map<string, mixed> $server): string
    {
        // IIS7 with URL Rewrite: make sure we get the unencoded url
        // (double slash problem).
        $iisUrlRewritten = $server->contains('IIS_WasUrlRewritten') ? (string) $server->get('IIS_WasUrlRewritten') : null;
        $unencodedUrl    = (string) $server->get('UNENCODED_URL');

        if ('1' === $iisUrlRewritten && '' !== $unencodedUrl) {
            return $unencodedUrl;
        }

        $requestUri = $server->get('REQUEST_URI');
        if (null !== $requestUri) {
            return preg_replace('#^[^/:]+://[^/]+#', '', (string) $requestUri);
        }

        $origPathInfo = $server->get('ORIG_PATH_INFO');

        if (null === $origPathInfo || '' === $origPathInfo) {
            return '/';
        }

        return (string) $origPathInfo;
    }

    private function marshalHttpsValue(mixed $https): bool
    {
        if ($https is bool) {
            return $https;
        }

        if (!$https is string) {
            throw new Exception\InvalidArgumentException(Str\format(
                'SAPI HTTPS value MUST be a string or boolean; received %s',
                gettype($https)
            ));
        }

        return '' !== $https && 'off' !== Str\lowercase($https);
    }

    private function getHeadersFromMap(string $name, Map<string, Set<string>> $headers, mixed $default = null): mixed
    {
        $header  = Str\lowercase($name);

        foreach ($headers as $key => $value) {
            if (Str\lowercase($key) === $header) {
                return Str\join($value->toArray(),', ');
            }
        }

        return $default;
    }
}
