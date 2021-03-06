<?hh

namespace Waffle\Tests\Http\Message;

use type Facebook\HackTest\HackTest;
use function Facebook\FBExpect\expect;
use namespace Waffle\Http\Message;
use type Waffle\Http\Message\Uri;

class UriTest extends HackTest
{
    const RFC3986_BASE = 'http://a/b/c/d;p?q';

    public function testParsesProvidedUri()
    {
        $uri = new Uri('https://user:pass@example.com:8080/path/123?q=abc#test');

        expect($uri->getScheme())->toBeSame('https');
        expect($uri->getAuthority())->toBeSame('user:pass@example.com:8080');
        expect($uri->getUserInfo())->toBeSame('user:pass');
        expect($uri->getHost())->toBeSame('example.com');
        expect($uri->getPort())->toBeSame(8080);
        expect($uri->getPath())->toBeSame('/path/123');
        expect($uri->getQuery())->toBeSame('q=abc');
        expect($uri->getFragment())->toBeSame('test');
        expect((string) $uri)->toBeSame('https://user:pass@example.com:8080/path/123?q=abc#test');
    }

    public function testCanTransformAndRetrievePartsIndividually()
    {
        $uri = (new Uri())
            ->withScheme('https')
            ->withUserInfo('user', 'pass')
            ->withHost('example.com')
            ->withPort(8080)
            ->withPath('/path/123')
            ->withQuery('q=abc')
            ->withFragment('test');

        expect($uri->getScheme())->toBeSame('https');
        expect($uri->getAuthority())->toBeSame('user:pass@example.com:8080');
        expect($uri->getUserInfo())->toBeSame('user:pass');
        expect($uri->getHost())->toBeSame('example.com');
        expect($uri->getPort())->toBeSame(8080);
        expect($uri->getPath())->toBeSame('/path/123');
        expect($uri->getQuery())->toBeSame('q=abc');
        expect($uri->getFragment())->toBeSame('test');
        expect((string) $uri)->toBeSame('https://user:pass@example.com:8080/path/123?q=abc#test');
    }

    <<DataProvider('getValidUris')>>
    public function testValidUrisStayValid($input)
    {
        $uri = new Uri($input);

        expect((string) $uri)->toBeSame($input);
    }

    public function getValidUris()
    {
        return [
            ['urn:path-rootless'],
            ['urn:path:with:colon'],
            ['urn:/path-absolute'],
            ['urn:/'],
            // only scheme with empty path
            ['urn:'],
            // only path
            ['/'],
            ['relative/'],
            ['0'],
            // same document reference
            [''],
            // network path without scheme
            ['//example.org'],
            ['//example.org/'],
            ['//example.org?q#h'],
            // only query
            ['?q'],
            ['?q=abc&foo=bar'],
            // only fragment
            ['#fragment'],
            // dot segments are not removed automatically
            ['./foo/../bar'],
        ];
    }

    <<DataProvider('getInvalidUris')>>
    public function testInvalidUrisThrowException($invalidUri)
    {
        expect(() ==> {
            new Uri($invalidUri);
        })->toThrow(
            Message\Exception\InvalidArgumentException::class,
            'Unable to parse URI'
        );
    }

    public function getInvalidUris()
    {
        return [
            // parse_url() requires the host component which makes sense for http(s)
            // but not when the scheme is not known or different. So '//' or '///' is
            // currently invalid as well but should not according to RFC 3986.
            ['http://'],
            ['urn://host:with:colon'], // host cannot contain ":"
        ];
    }

    public function testPortMustBeValid()
    {
        expect(() ==> {
            (new Uri())->withPort(100000);
        })->toThrow(
            Message\Exception\InvalidArgumentException::class,
            'Invalid port: 100000. Must be between 1 and 65535'
        );
    }

    public function testWithPortCannotBeZero()
    {
        expect(() ==> {
            (new Uri())->withPort(0);
        })->toThrow(
            Message\Exception\InvalidArgumentException::class,
            'Invalid port: 0. Must be between 1 and 65535'
        );
    }

    public function testParseUriPortCannotBeZero()
    {
        expect(() ==> {
            new Uri('//example.com:0');
        })->toThrow(
            Message\Exception\InvalidArgumentException::class,
            'Unable to parse URI'
        );
    }

    public function testCanParseFalseyUriParts()
    {
        $uri = new Uri('0://0:0@0/0?0#0');

        expect($uri->getScheme())->toBeSame('0');
        expect($uri->getAuthority())->toBeSame('0:0@0');
        expect($uri->getUserInfo())->toBeSame('0:0');
        expect($uri->getHost())->toBeSame('0');
        expect($uri->getPath())->toBeSame('/0');
        expect($uri->getQuery())->toBeSame('0');
        expect($uri->getFragment())->toBeSame('0');
        expect((string) $uri)->toBeSame('0://0:0@0/0?0#0');
    }

    public function testCanConstructFalseyUriParts()
    {
        $uri = (new Uri())
            ->withScheme('0')
            ->withUserInfo('0', '0')
            ->withHost('0')
            ->withPath('/0')
            ->withQuery('0')
            ->withFragment('0');

        expect($uri->getScheme())->toBeSame('0');
        expect($uri->getAuthority())->toBeSame('0:0@0');
        expect($uri->getUserInfo())->toBeSame('0:0');
        expect($uri->getHost())->toBeSame('0');
        expect($uri->getPath())->toBeSame('/0');
        expect($uri->getQuery())->toBeSame('0');
        expect($uri->getFragment())->toBeSame('0');
        expect((string) $uri)->toBeSame('0://0:0@0/0?0#0');
    }

    public function getResolveTestCases()
    {
        return [
            [self::RFC3986_BASE, 'g:h',           'g:h'],
            [self::RFC3986_BASE, 'g',             'http://a/b/c/g'],
            [self::RFC3986_BASE, './g',           'http://a/b/c/g'],
            [self::RFC3986_BASE, 'g/',            'http://a/b/c/g/'],
            [self::RFC3986_BASE, '/g',            'http://a/g'],
            [self::RFC3986_BASE, '//g',           'http://g'],
            [self::RFC3986_BASE, '?y',            'http://a/b/c/d;p?y'],
            [self::RFC3986_BASE, 'g?y',           'http://a/b/c/g?y'],
            [self::RFC3986_BASE, '#s',            'http://a/b/c/d;p?q#s'],
            [self::RFC3986_BASE, 'g#s',           'http://a/b/c/g#s'],
            [self::RFC3986_BASE, 'g?y#s',         'http://a/b/c/g?y#s'],
            [self::RFC3986_BASE, ';x',            'http://a/b/c/;x'],
            [self::RFC3986_BASE, 'g;x',           'http://a/b/c/g;x'],
            [self::RFC3986_BASE, 'g;x?y#s',       'http://a/b/c/g;x?y#s'],
            [self::RFC3986_BASE, '',              self::RFC3986_BASE],
            [self::RFC3986_BASE, '.',             'http://a/b/c/'],
            [self::RFC3986_BASE, './',            'http://a/b/c/'],
            [self::RFC3986_BASE, '..',            'http://a/b/'],
            [self::RFC3986_BASE, '../',           'http://a/b/'],
            [self::RFC3986_BASE, '../g',          'http://a/b/g'],
            [self::RFC3986_BASE, '../..',         'http://a/'],
            [self::RFC3986_BASE, '../../',        'http://a/'],
            [self::RFC3986_BASE, '../../g',       'http://a/g'],
            [self::RFC3986_BASE, '../../../g',    'http://a/g'],
            [self::RFC3986_BASE, '../../../../g', 'http://a/g'],
            [self::RFC3986_BASE, '/./g',          'http://a/g'],
            [self::RFC3986_BASE, '/../g',         'http://a/g'],
            [self::RFC3986_BASE, 'g.',            'http://a/b/c/g.'],
            [self::RFC3986_BASE, '.g',            'http://a/b/c/.g'],
            [self::RFC3986_BASE, 'g..',           'http://a/b/c/g..'],
            [self::RFC3986_BASE, '..g',           'http://a/b/c/..g'],
            [self::RFC3986_BASE, './../g',        'http://a/b/g'],
            [self::RFC3986_BASE, 'foo////g',      'http://a/b/c/foo////g'],
            [self::RFC3986_BASE, './g/.',         'http://a/b/c/g/'],
            [self::RFC3986_BASE, 'g/./h',         'http://a/b/c/g/h'],
            [self::RFC3986_BASE, 'g/../h',        'http://a/b/c/h'],
            [self::RFC3986_BASE, 'g;x=1/./y',     'http://a/b/c/g;x=1/y'],
            [self::RFC3986_BASE, 'g;x=1/../y',    'http://a/b/c/y'],
            // dot-segments in the query or fragment
            [self::RFC3986_BASE, 'g?y/./x',       'http://a/b/c/g?y/./x'],
            [self::RFC3986_BASE, 'g?y/../x',      'http://a/b/c/g?y/../x'],
            [self::RFC3986_BASE, 'g#s/./x',       'http://a/b/c/g#s/./x'],
            [self::RFC3986_BASE, 'g#s/../x',      'http://a/b/c/g#s/../x'],
            [self::RFC3986_BASE, 'g#s/../x',      'http://a/b/c/g#s/../x'],
            [self::RFC3986_BASE, '?y#s',          'http://a/b/c/d;p?y#s'],
            ['http://a/b/c/d;p?q#s', '?y',        'http://a/b/c/d;p?y'],
            ['http://u@a/b/c/d;p?q', '.',         'http://u@a/b/c/'],
            ['http://u:p@a/b/c/d;p?q', '.',       'http://u:p@a/b/c/'],
            ['http://a/b/c/d/', 'e',              'http://a/b/c/d/e'],
            ['urn:no-slash', 'e',                 'urn:e'],
            // falsey relative parts
            [self::RFC3986_BASE, '//0',           'http://0'],
            [self::RFC3986_BASE, '0',             'http://a/b/c/0'],
            [self::RFC3986_BASE, '?0',            'http://a/b/c/d;p?0'],
            [self::RFC3986_BASE, '#0',            'http://a/b/c/d;p?q#0'],
        ];
    }

    public function testSchemeIsNormalizedToLowercase()
    {
        $uri = new Uri('HTTP://example.com');

        expect($uri->getScheme())->toBeSame('http');
        expect((string) $uri)->toBeSame('http://example.com');

        $uri = (new Uri('//example.com'))->withScheme('HTTP');

        expect($uri->getScheme())->toBeSame('http');
        expect((string) $uri)->toBeSame('http://example.com');
    }

    public function testHostIsNormalizedToLowercase()
    {
        $uri = new Uri('//eXaMpLe.CoM');

        expect($uri->getHost())->toBeSame('example.com');
        expect((string) $uri)->toBeSame('//example.com');

        $uri = (new Uri())->withHost('eXaMpLe.CoM');

        expect($uri->getHost())->toBeSame('example.com');
        expect((string) $uri)->toBeSame('//example.com');
    }

    public function testPortIsNullIfStandardPortForScheme()
    {
        // HTTPS standard port
        $uri = new Uri('https://example.com:443');
        expect($uri->getPort())->toBeNull();
        expect($uri->getAuthority())->toBeSame('example.com');

        $uri = (new Uri('https://example.com'))->withPort(443);
        expect($uri->getPort())->toBeNull();
        expect($uri->getAuthority())->toBeSame('example.com');

        // HTTP standard port
        $uri = new Uri('http://example.com:80');
        expect($uri->getPort())->toBeNull();
        expect($uri->getAuthority())->toBeSame('example.com');

        $uri = (new Uri('http://example.com'))->withPort(80);
        expect($uri->getPort())->toBeNull();
        expect($uri->getAuthority())->toBeSame('example.com');
    }

    public function testPortIsReturnedIfSchemeUnknown()
    {
        $uri = (new Uri('//example.com'))->withPort(80);

        expect($uri->getPort())->toBeSame(80);
        expect($uri->getAuthority())->toBeSame('example.com:80');
    }

    public function testStandardPortIsNullIfSchemeChanges()
    {
        $uri = new Uri('http://example.com:443');
        expect($uri->getScheme())->toBeSame('http');
        expect($uri->getPort())->toBeSame(443);

        $uri = $uri->withScheme('https');
        expect($uri->getPort())->toBeNull();
    }

    public function testPortCanBeRemoved()
    {
        $uri = (new Uri('http://example.com:8080'))->withPort(null);

        expect($uri->getPort())->toBeNull();
        expect((string) $uri)->toBeSame('http://example.com');
    }

    public function testAuthorityWithUserInfoButWithoutHost()
    {
        $uri = (new Uri())->withUserInfo('user', 'pass');

        expect($uri->getUserInfo())->toBeSame('user:pass');
        expect($uri->getAuthority())->toBeSame('');
    }

    public function uriComponentsEncodingProvider()
    {
        $unreserved = 'a-zA-Z0-9.-_~!$&\'()*+,;=:@';

        return [
            // Percent encode spaces
            ['/pa th?q=va lue#frag ment', '/pa%20th', 'q=va%20lue', 'frag%20ment', '/pa%20th?q=va%20lue#frag%20ment'],
            // Percent encode multibyte
            ['/€?€#€', '/%E2%82%AC', '%E2%82%AC', '%E2%82%AC', '/%E2%82%AC?%E2%82%AC#%E2%82%AC'],
            // Don't encode something that's already encoded
            ['/pa%20th?q=va%20lue#frag%20ment', '/pa%20th', 'q=va%20lue', 'frag%20ment', '/pa%20th?q=va%20lue#frag%20ment'],
            // Percent encode invalid percent encodings
            ['/pa%2-th?q=va%2-lue#frag%2-ment', '/pa%252-th', 'q=va%252-lue', 'frag%252-ment', '/pa%252-th?q=va%252-lue#frag%252-ment'],
            // Don't encode path segments
            ['/pa/th//two?q=va/lue#frag/ment', '/pa/th//two', 'q=va/lue', 'frag/ment', '/pa/th//two?q=va/lue#frag/ment'],
            // Don't encode unreserved chars or sub-delimiters
            ["/$unreserved?$unreserved#$unreserved", "/$unreserved", $unreserved, $unreserved, "/$unreserved?$unreserved#$unreserved"],
            // Encoded unreserved chars are not decoded
            ['/p%61th?q=v%61lue#fr%61gment', '/p%61th', 'q=v%61lue', 'fr%61gment', '/p%61th?q=v%61lue#fr%61gment'],
        ];
    }

    <<DataProvider('uriComponentsEncodingProvider')>>
    public function testUriComponentsGetEncodedProperly($input, $path, $query, $fragment, $output)
    {
        $uri = new Uri($input);
        expect($uri->getPath())->toBeSame($path);
        expect($uri->getQuery())->toBeSame($query);
        expect($uri->getFragment())->toBeSame($fragment);
        expect((string) $uri)->toBeSame($output);
    }

    public function testWithPathEncodesProperly()
    {
        $uri = (new Uri())->withPath('/baz?#€/b%61r');
        // Query and fragment delimiters and multibyte chars are encoded.
        expect($uri->getPath())->toBeSame('/baz%3F%23%E2%82%AC/b%61r');
        expect((string) $uri)->toBeSame('/baz%3F%23%E2%82%AC/b%61r');
    }

    public function testWithQueryEncodesProperly()
    {
        $uri = (new Uri())->withQuery('?=#&€=/&b%61r');
        // A query starting with a "?" is valid and must not be magically removed. Otherwise it would be impossible to
        // construct such an URI. Also the "?" and "/" does not need to be encoded in the query.
        expect($uri->getQuery())->toBeSame('?=%23&%E2%82%AC=/&b%61r');
        expect((string) $uri)->toBeSame('??=%23&%E2%82%AC=/&b%61r');
    }

    public function testWithFragmentEncodesProperly()
    {
        $uri = (new Uri())->withFragment('#€?/b%61r');
        // A fragment starting with a "#" is valid and must not be magically removed. Otherwise it would be impossible to
        // construct such an URI. Also the "?" and "/" does not need to be encoded in the fragment.
        expect($uri->getFragment())->toBeSame('%23%E2%82%AC?/b%61r');
        expect((string) $uri)->toBeSame('#%23%E2%82%AC?/b%61r');
    }

    public function testAllowsForRelativeUri()
    {
        $uri = (new Uri())->withPath('foo');
        expect($uri->getPath())->toBeSame('foo');
        expect((string) $uri)->toBeSame('foo');
    }

    public function testAddsSlashForRelativeUriStringWithHost()
    {
        // If the path is rootless and an authority is present, the path MUST
        // be prefixed by "/".
        $uri = (new Uri())->withPath('foo')->withHost('example.com');
        expect($uri->getPath())->toBeSame('foo');
        // concatenating a relative path with a host doesn't work: "//example.comfoo" would be wrong
        expect((string) $uri)->toBeSame('//example.com/foo');
    }

    public function testRemoveExtraSlashesWihoutHost()
    {
        // If the path is starting with more than one "/" and no authority is
        // present, the starting slashes MUST be reduced to one.
        $uri = (new Uri())->withPath('//foo');
        expect($uri->getPath())->toBeSame('//foo');
        // URI "//foo" would be interpreted as network reference and thus change the original path to the host
        expect((string) $uri)->toBeSame('/foo');
    }

    public function testDefaultReturnValuesOfGetters()
    {
        $uri = new Uri();

        expect($uri->getScheme())->toBeSame('');
        expect($uri->getAuthority())->toBeSame('');
        expect($uri->getUserInfo())->toBeSame('');
        expect($uri->getHost())->toBeSame('');
        expect($uri->getPort())->toBeNull();
        expect($uri->getPath())->toBeSame('');
        expect($uri->getQuery())->toBeSame('');
        expect($uri->getFragment())->toBeSame('');
    }

    public function testImmutability()
    {
        $uri = new Uri();

        expect($uri->withScheme('https'))->toNotBeSame($uri);
        expect($uri->withUserInfo('user', 'pass'))->toNotBeSame($uri);
        expect($uri->withHost('example.com'))->toNotBeSame($uri);
        expect($uri->withPort(8080))->toNotBeSame($uri);
        expect($uri->withPath('/path/123'))->toNotBeSame($uri);
        expect($uri->withQuery('q=abc'))->toNotBeSame($uri);
        expect($uri->withFragment('test'))->toNotBeSame($uri);
    }
}
