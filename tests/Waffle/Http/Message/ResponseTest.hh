<?hh

namespace Waffle\Tests\Http\Message;

use Facebook\HackTest\HackTest;
use Waffle\Contract\Http\Message\StreamInterface;
use Waffle\Http\Message\Response;
use Waffle\Http\Message\Cookie;
use Waffle\Http\Message\__Private;
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
        $r = new Response(200, Map { 'Foo' => Set { 'Bar' } });
        expect($r->getHeaders())->toBePHPEqual(Map { 'Foo' => Set { 'Bar' } });
        expect($r->getHeaderLine('Foo'))->toBeSame('Bar');
        expect($r->getHeader('Foo'))->toBePHPEqual(Set { 'Bar' });
    }
    public function testCanConstructWithBody()
    {
        $r = new Response(200, Map {}, __Private\create_stream_from_string('baz'));
        expect($r->getBody())->toBeInstanceOf(StreamInterface::class);
        expect((string) $r->getBody())->toBeSame('baz');
    }

    public function testNullBody()
    {
        $r = new Response(200, Map {}, null);
        expect($r->getBody())->toBeInstanceOf(StreamInterface::class);
        expect((string) $r->getBody())->toBeSame('');
    }

    public function testCanConstructWithReason()
    {
        $r = new Response(200, Map {}, null, '1.1', 'bar');
        expect($r->getReasonPhrase())->toBeSame('bar');

        $r = new Response(200, Map {}, null, '1.1', '0');
        expect($r->getReasonPhrase())->toBeSame('0', 'Falsey reason works');
    }

    public function testCanConstructWithProtocolVersion()
    {
        $r = new Response(200, Map {}, null, '1000');
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
        $r = new Response(200, Map { 'Foo' => Set { 'Bar' }});
        $r2 = $r->withHeader('baZ', Set { 'Bam' });
        expect($r->getHeaders())->toBePHPEqual(Map { 'Foo' => Set { 'Bar' }});
        expect($r2->getHeaders())->toBePHPEqual(Map { 'Foo' => Set { 'Bar' }, 'baZ' => Set {'Bam'}});
        expect($r2->getHeaderLine('baz'))->toBeSame('Bam');
        expect($r2->getHeader('baz'))->toBePHPEqual(Set { 'Bam' });
    }

    public function testWithHeaderReplacesDifferentCase()
    {
        $r = new Response(200, Map {'Foo' => Set {'Bar'}});
        $r2 = $r->withHeader('foO', Set {'Bam'});
        expect($r->getHeaders())->toBePHPEqual(Map { 'Foo' => Set { 'Bar' }});
        expect($r2->getHeaders())->toBePHPEqual(Map { 'foO' => Set { 'Bam' }});
        expect($r2->getHeaderLine('foo'))->toBeSame('Bam');
        expect($r2->getHeader('foo'))->toBePHPEqual(Set { 'Bam' });
    }

    public function testWithAddedHeader()
    {
        $r = new Response(200, Map { 'Foo' => Set { 'Bar' }});
        $r2 = $r->withAddedHeader('foO', Set { 'Baz' });
        expect($r->getHeaders())->toBePHPEqual(Map { 'Foo' => Set {'Bar'} });
        expect($r2->getHeaders())->toBePHPEqual(Map { 'Foo' => Set{'Bar', 'Baz'} });
        expect($r2->getHeaderLine('foo'))->toBeSame('Bar, Baz');
        expect($r2->getHeader('foo'))->toBePHPEqual(Set {'Bar', 'Baz'});
    }

    public function testWithAddedHeaderAsArray()
    {
        $r = new Response(200, Map { 'Foo' => Set { 'Bar' }});
        $r2 = $r->withAddedHeader('foO', Set { 'Baz', 'Bam' });
        expect($r->getHeaders())->toBePHPEqual(Map { 'Foo' => Set { 'Bar' }});
        expect($r2->getHeaders())->toBePHPEqual(Map { 'Foo' => Set {'Bar', 'Baz', 'Bam'} });
        expect($r2->getHeaderLine('foo'))->toBeSame('Bar, Baz, Bam');
        expect($r2->getHeader('foo'))->toBePHPEqual(Set {'Bar', 'Baz', 'Bam'});
    }

    public function testWithAddedHeaderThatDoesNotExist()
    {
        $r = new Response(200, Map { 'Foo' => Set { 'Bar' } 
        });
        $r2 = $r->withAddedHeader('nEw', Set { 'Baz' });
        expect($r->getHeaders())->toBePHPEqual(Map { 'Foo' => Set {'Bar'} });
        expect($r2->getHeaders())->toBePHPEqual(Map { 'Foo' => Set {'Bar'}, 'nEw' => Set {'Baz'} });
        expect($r2->getHeaderLine('new'))->toBeSame('Baz');
        expect($r2->getHeader('new'))->toBePHPEqual(Set { 'Baz' });
    }

    public function testWithoutHeaderThatExists()
    {
        $r = new Response(200, Map { 'Foo' => Set {'Bar'}, 'Baz' => Set {'Bam'} });
        $r2 = $r->withoutHeader('foO');
        expect($r->hasHeader('foo'))->toBeTrue();
        expect($r->getHeaders())->toBePHPEqual( Map { 'Foo' => Set {'Bar'}, 'Baz' => Set {'Bam'} });
        expect($r2->hasHeader('foo'))->toBeFalse();
        expect($r2->getHeaders())->toBePHPEqual(Map { 'Baz' => Set {'Bam'} });
    }

    public function testWithoutHeaderThatDoesNotExist()
    {
        $r = new Response(200, Map { 'Baz' => Set { 'Bam' }});
        $r2 = $r->withoutHeader('foO');
        expect($r2)->toBeSame($r);
        expect($r2->hasHeader('foo'))->toBeFalse();
        expect($r2->getHeaders())->toBePHPEqual(Map { 'Baz' => Set { 'Bam' } });
    }

    public function testSameInstanceWhenRemovingMissingHeader()
    {
        $r = new Response();
        expect($r->withoutHeader('foo'))->toBeSame($r);
    }

    public function trimmedHeaderValues()
    {
        return [
            [new Response(200, Map { 'OWS' => Set { " \t \tFoo\t \t " } })],
            [(new Response())->withHeader('OWS', Set { " \t \tFoo\t \t " })],
            [(new Response())->withAddedHeader('OWS', Set { " \t \tFoo\t \t " })],
        ];
    }

    <<DataProvider('trimmedHeaderValues')>>
    public function testHeaderValuesAreTrimmed($r)
    {
        expect($r->getHeaders())->toBePHPEqual(Map { 'OWS' => Set { 'Foo' } });
        expect($r->getHeaderLine('OWS'))->toBePHPEqual('Foo');
        expect($r->getHeader('OWS'))->toBePHPEqual(Set { 'Foo' });
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
        expect($response2->getCookies()->count())->toBeSame(1);
        expect($response2->getCookies()->at('name1'))->toBeSame($cookie1);

        $response3 = $response2
                        ->withCookie('name2', $cookie2)
                        ->withCookie('name3', $cookie3);

        expect($response3)->toNotBeSame($response2);
        expect($response3->getCookies()->count())->toBeSame(3);
        expect($response3->getCookies()->at('name1'))->toBeSame($cookie1);
        expect($response3->getCookies()->at('name2'))->toBeSame($cookie2);
        expect($response3->getCookies()->at('name3'))->toBeSame($cookie3);
    }
}
