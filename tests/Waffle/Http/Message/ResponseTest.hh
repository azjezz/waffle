<?hh

namespace Waffle\Tests\Http\Message;

use namespace HH\Lib\C;
use namespace Waffle\Http\Message\__Private;
use type Facebook\HackTest\HackTest;
use type Waffle\Contract\Http\Message\StreamInterface;
use type Waffle\Http\Message\Response;
use type Waffle\Http\Message\Cookie;
use function Facebook\FBExpect\expect;

class ResponseTest extends HackTest
{
    public function testDefaultConstructor()
    {
        $r = new Response();
        expect($r->getStatusCode())->toBeSame(200);
        expect($r->getProtocolVersion())->toBeSame('1.1');
        expect($r->getReasonPhrase())->toBeSame('OK');
        expect($r->getHeaders())->toBeEmpty();
        expect($r->getBody())->toBeInstanceOf(StreamInterface::class);
        expect((string) $r->getBody())->toBeSame('');
    }

    public function testCanConstructWithStatusCode()
    {
        $r = new Response(404);
        expect($r->getStatusCode())->toBeSame(404);
        expect($r->getReasonPhrase())->toBeSame('Not Found');
    }

    public function testCanConstructWithHeaders()
    {
        $r = new Response(200, dict[ 'Foo' => vec[ 'Bar' ]]);
        expect($r->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec[ 'Bar' ]]);
        expect($r->getHeaderLine('Foo'))->toBeSame('Bar');
        expect($r->getHeader('Foo'))->toBePHPEqual(vec[ 'Bar' ]);
    }
    public function testCanConstructWithBody()
    {
        $r = new Response(200, dict[], __Private\create_stream_from_string('baz'));
        expect($r->getBody())->toBeInstanceOf(StreamInterface::class);
        expect((string) $r->getBody())->toBeSame('baz');
    }

    public function testNullBody()
    {
        $r = new Response(200, dict[], null);
        expect($r->getBody())->toBeInstanceOf(StreamInterface::class);
        expect((string) $r->getBody())->toBeSame('');
    }

    public function testCanConstructWithReason()
    {
        $r = new Response(200, dict[], null, '1.1', 'bar');
        expect($r->getReasonPhrase())->toBeSame('bar');

        $r = new Response(200, dict[], null, '1.1', '0');
        expect($r->getReasonPhrase())->toBeSame('0', 'Falsey reason works');
    }

    public function testCanConstructWithProtocolVersion()
    {
        $r = new Response(200, dict[], null, '1000');
        expect($r->getProtocolVersion())->toBeSame('1000');
    }

    public function testWithStatusCodeAndNoReason()
    {
        $r = (new Response())->withStatus(201);
        expect($r->getStatusCode())->toBeSame(201);
        expect($r->getReasonPhrase())->toBeSame('Created');
    }

    public function testWithStatusCodeAndReason()
    {
        $r = (new Response())->withStatus(201, 'Foo');
        expect($r->getStatusCode())->toBeSame(201);
        expect($r->getReasonPhrase())->toBeSame('Foo');

        $r = (new Response())->withStatus(201, '0');
        expect($r->getStatusCode())->toBeSame(201);
        expect($r->getReasonPhrase())->toBeSame('0', 'Falsey reason works');
    }

    public function testWithProtocolVersion()
    {
        $r = (new Response())->withProtocolVersion('1000');
        expect($r->getProtocolVersion())->toBeSame('1000');
    }

    public function testSameInstanceWhenSameProtocol()
    {
        $r = new Response();
        expect($r->withProtocolVersion('1.1'))->toBeSame($r);
    }

    public function testWithBody()
    {
        $b = __Private\create_stream_from_string('0');
        $r = (new Response())->withBody($b);
        expect($r->getBody())->toBeInstanceOf(StreamInterface::class);
        expect((string) $r->getBody())->toBeSame('0');
    }

    public function testSameInstanceWhenSameBody()
    {
        $r = new Response();
        $b = $r->getBody();
        expect($r->withBody($b))->toBeSame($r);
    }

    public function testWithHeader()
    {
        $r = new Response(200, dict[ 'Foo' => vec[ 'Bar' ]]);
        $r2 = $r->withHeader('baZ', vec[ 'Bam' ]);
        expect($r->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec[ 'Bar' ]]);
        expect($r2->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec[ 'Bar' ], 'baZ' => vec[ 'Bam' ]]);
        expect($r2->getHeaderLine('baz'))->toBeSame('Bam');
        expect($r2->getHeader('baz'))->toBePHPEqual(vec[ 'Bam' ]);
    }

    public function testWithHeaderReplacesDifferentCase()
    {
        $r = new Response(200, dict['Foo' => vec['Bar']]);
        $r2 = $r->withHeader('foO', vec['Bam']);
        expect($r->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec[ 'Bar' ] ]);
        expect($r2->getHeaders())->toBePHPEqual(dict[ 'foO' => vec[ 'Bam' ]]);
        expect($r2->getHeaderLine('foo'))->toBeSame('Bam');
        expect($r2->getHeader('foo'))->toBePHPEqual(vec[ 'Bam' ]);
    }

    public function testWithAddedHeader()
    {
        $r = new Response(200, dict[ 'Foo' => vec[ 'Bar' ]]);
        $r2 = $r->withAddedHeader('foO', vec[ 'Baz' ]);
        expect($r->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec['Bar']]);
        expect($r2->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec['Bar', 'Baz']]);
        expect($r2->getHeaderLine('foo'))->toBeSame('Bar, Baz');
        expect($r2->getHeader('foo'))->toBePHPEqual(vec['Bar', 'Baz']);
    }

    public function testWithAddedHeaderAsArray()
    {
        $r = new Response(200, dict[ 'Foo' => vec[ 'Bar' ]]);
        $r2 = $r->withAddedHeader('foO', vec[ 'Baz', 'Bam' ]);
        expect($r->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec[ 'Bar' ]]);
        expect($r2->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec['Bar', 'Baz', 'Bam']]);
        expect($r2->getHeaderLine('foo'))->toBeSame('Bar, Baz, Bam');
        expect($r2->getHeader('foo'))->toBePHPEqual(vec['Bar', 'Baz', 'Bam']);
    }

    public function testWithAddedHeaderThatDoesNotExist()
    {
        $r = new Response(200, dict[ 'Foo' => vec[ 'Bar']]);
        $r2 = $r->withAddedHeader('nEw', vec[ 'Baz' ]);
        expect($r->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec['Bar']]);
        expect($r2->getHeaders())->toBePHPEqual(dict[ 'Foo' => vec['Bar'], 'nEw' => vec['Baz']]);
        expect($r2->getHeaderLine('new'))->toBeSame('Baz');
        expect($r2->getHeader('new'))->toBePHPEqual(vec[ 'Baz' ]);
    }

    public function testWithoutHeaderThatExists()
    {
        $r = new Response(200, dict[ 'Foo' => vec['Bar'], 'Baz' => vec['Bam']]);
        $r2 = $r->withoutHeader('foO');
        expect($r->hasHeader('foo'))->toBeTrue();
        expect($r->getHeaders())->toBePHPEqual( dict[ 'Foo' => vec['Bar'], 'Baz' => vec['Bam']]);
        expect($r2->hasHeader('foo'))->toBeFalse();
        expect($r2->getHeaders())->toBePHPEqual(dict[ 'Baz' => vec['Bam']]);
    }

    public function testWithoutHeaderThatDoesNotExist()
    {
        $r = new Response(200, dict[ 'Baz' => vec[ 'Bam' ]]);
        $r2 = $r->withoutHeader('foO');
        expect($r2)->toBeSame($r);
        expect($r2->hasHeader('foo'))->toBeFalse();
        expect($r2->getHeaders())->toBePHPEqual(dict[ 'Baz' => vec[ 'Bam']]);
    }

    public function testSameInstanceWhenRemovingMissingHeader()
    {
        $r = new Response();
        expect($r->withoutHeader('foo'))->toBeSame($r);
    }

    public function trimmedHeaderValues()
    {
        return [
            [new Response(200, dict[ 'OWS' => vec[ " \t \tFoo\t \t " ]])],
            [(new Response())->withHeader('OWS', vec[ " \t \tFoo\t \t " ])],
            [(new Response())->withAddedHeader('OWS', vec[ " \t \tFoo\t \t " ])],
        ];
    }

    <<DataProvider('trimmedHeaderValues')>>
    public function testHeaderValuesAreTrimmed($r)
    {
        expect($r->getHeaders())->toBePHPEqual(dict[ 'OWS' => vec[ 'Foo' ]]);
        expect($r->getHeaderLine('OWS'))->toBePHPEqual('Foo');
        expect($r->getHeader('OWS'))->toBePHPEqual(vec[ 'Foo' ]);
    }

    public function testWithAndWithoutCookie()
    {
        $response = new Response();
        $cookie = new Cookie('value');
        $response2 = $response->withCookie('name', $cookie);

        expect($response2)->toNotBeSame($response);

        expect($response->getCookie('name'))->toBeNull();
        expect($response2->getCookie('name'))->toBeSame($cookie);

        $response3 = $response2->withoutCookie('name');

        expect($response3)->toNotBeSame($response2);
        expect($response3->getCookie('name'))->toBeNull();
    }

    public function testGetCookies()
    {
        $response = new Response();
        $cookie1 = new Cookie('value1');
        $cookie2 = new Cookie('value2');
        $cookie3 = new Cookie('value3');
        $response2 = $response->withCookie('name1', $cookie1);

        expect($response2)->toNotBeSame($response);
        expect(C\count($response2->getCookies()))->toBeSame(1);
        expect($response2->getCookies()['name1'])->toBeSame($cookie1);

        $response3 = $response2
                        ->withCookie('name2', $cookie2)
                        ->withCookie('name3', $cookie3);

        expect($response3)->toNotBeSame($response2);
        expect(C\count($response3->getCookies()))->toBeSame(3);
        expect($response3->getCookies()['name1'])->toBeSame($cookie1);
        expect($response3->getCookies()['name2'])->toBeSame($cookie2);
        expect($response3->getCookies()['name3'])->toBeSame($cookie3);
    }
}
