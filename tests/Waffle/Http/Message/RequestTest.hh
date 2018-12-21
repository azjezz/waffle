<?hh

namespace Waffle\Tests\Http\Message;

use type Waffle\Http\Message\Request;
use namespace Waffle\Http\Message\Exception;
use function Facebook\FBExpect\expect;
use type Waffle\Http\Message\Uri;
use type Facebook\HackTest\HackTest;
use type Waffle\Contract\Http\Message\StreamInterface;

class RequestTest extends HackTest
{
    public function testRequestUriMayBeUri()
    {
        $uri = new Uri('/');
        $r = new Request('GET', $uri);
        expect($r->getUri())->toBeSame($uri);
    }

    public function testNullBody()
    {
        $r = new Request('GET', new Uri('/'), dict[], null);
        expect($r->getBody())->toBeInstanceOf(StreamInterface::class);
        expect((string) $r->getBody())->toBeSame('');
    }

    public function testWithUri()
    {
        $r1 = new Request('GET', new Uri('/'));
        $u1 = $r1->getUri();
        $u2 = new Uri('http://www.example.com');
        $r2 = $r1->withUri($u2);
        expect($r2)->toNotBeSame($r1);
        expect($r2->getUri())->toBeSame($u2);
        expect($r1->getUri())->toBeSame($u1);
    }

    public function testSameInstanceWhenSameUri()
    {
        $r1 = new Request('GET', new Uri('http://foo.com'));
        $r2 = $r1->withUri($r1->getUri());
        expect($r2)->toBeSame($r1);
    }

    public function testWithRequestTarget()
    {
        $r1 = new Request('GET', new Uri('/'));
        $r2 = $r1->withRequestTarget('*');
        expect($r2->getRequestTarget())->toBeSame('*');
        expect($r1->getRequestTarget())->toBeSame('/');
    }

    public function testRequestTargetDoesNotAllowSpaces()
    {
        expect(() ==> {
            $r1 = new Request('GET', new Uri('/'));
            $r1->withRequestTarget('/foo bar');
        })->toThrow(
            Exception\InvalidArgumentException::class,
            'Invalid request target provided; cannot contain whitespace'
        );
    }

    public function testRequestTargetDefaultsToSlash()
    {
        $r1 = new Request('GET', new Uri(''));
        expect($r1->getRequestTarget())->toBeSame('/');
        $r2 = new Request('GET', new Uri('*'));
        expect($r2->getRequestTarget())->toBeSame('*');
        $r3 = new Request('GET', new Uri('http://foo.com/bar baz/'));
        expect($r3->getRequestTarget())->toBeSame('/bar%20baz/');
    }

    public function testBuildsRequestTarget()
    {
        $r1 = new Request('GET', new Uri('http://foo.com/baz?bar=bam'));
        expect($r1->getRequestTarget())->toBeSame('/baz?bar=bam');
    }

    public function testBuildsRequestTargetWithFalseyQuery()
    {
        $r1 = new Request('GET', new Uri('http://foo.com/baz?0'));
        expect($r1->getRequestTarget())->toBeSame('/baz?0');
    }

    public function testHostIsAddedFirst()
    {
        $r = new Request('GET', new Uri('http://foo.com/baz?bar=bam'), dict[
            'Foo' => vec[ 'Bar' ]
        ]);
        expect($r->getHeaders())->toBeSame(dict[
            'Host' => vec[ 'foo.com' ],
            'Foo' => vec[ 'Bar' ],
        ]);
    }

    public function testCanGetHeaderAsCsv()
    {
        $r = new Request('GET', new Uri('http://foo.com/baz?bar=bam'), dict[
            'Foo' => vec[ 'a', 'b', 'c' ],
        ]);
        expect($r->getHeaderLine('Foo'))->toBeSame('a, b, c');
        expect($r->getHeaderLine('Bar'))->toBeSame('');
    }

    public function testHostIsNotOverwrittenWhenPreservingHost()
    {
        $r = new Request('GET', new Uri('http://foo.com/baz?bar=bam'), dict[
            'Host' => vec[
                'facebook.com'
            ]
        ]);
        expect($r->getHeaders())->toBeSame(dict['Host' => vec[ 'facebook.com' ]]);
        $r2 = $r->withUri(new Uri('http://www.messenger.com/t/azjezz'), true);
        expect($r2->getHeaderLine('Host'))->toBeSame('facebook.com');
    }

    public function testOverridesHostWithUri()
    {
        $r = new Request('GET', new Uri('https://docs.hhvm.com/hack?bar=bam'));
        expect($r->getHeaders())->toBeSame(dict[ 'Host' => vec[ 'docs.hhvm.com' ]]);
        $r2 = $r->withUri(new Uri('https://hacklang.org/tutorial.html'));
        expect($r2->getHeaderLine('Host'))->toBeSame('hacklang.org');
    }

    public function testUniqueAggregatesHeaders()
    {
        $r = new Request('GET', new Uri(''), dict[
            'ZOO' => vec[ 'zoobar' ],
            'zoo' => vec[ 'foobar', 'zoobar' ],
        ]);
        expect($r->getHeaders())->toBeSame(dict[ 'ZOO' => vec[ 'zoobar', 'foobar' ]]);
        expect($r->getHeaderLine('zoo'))->toBeSame('zoobar, foobar');
    }

    public function testAddsPortToHeader()
    {
        $r = new Request('GET', new Uri('http://foo.com:8124/bar'));
        expect($r->getHeaderLine('host'))->toBeSame('foo.com:8124');
    }

    public function testAddsPortToHeaderAndReplacePreviousPort()
    {
        $r = new Request('GET', new Uri('http://foo.com:8124/bar'));
        $r = $r->withUri(new Uri('http://foo.com:8125/bar'));
        expect($r->getHeaderLine('host'))->toBeSame('foo.com:8125');
    }

    public function testCannotHaveHeaderWithEmptyName()
    {
        expect(() ==> {
            $r = new Request('GET', new Uri('https://example.com/'));
            $r = $r->withHeader('', vec[ 'Bar' ]);
        })->toThrow(
            Exception\InvalidArgumentException::class,
            'Header name must be an RFC 7230 compatible string.'
        );
    }

    public function testCanHaveHeaderWithEmptyValue()
    {
        $r = new Request('GET', new Uri('https://example.com/'));
        $r = $r->withHeader('Foo', vec[ '' ]);
        expect($r->getHeader('Foo'))->toBeSame(vec[ '' ]);
    }
}
