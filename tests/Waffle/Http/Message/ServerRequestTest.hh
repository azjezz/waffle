<?hh

namespace Waffle\Tests\Http\Message;

use Waffle\Http\Message\ServerRequest;
use Waffle\Http\Message\Uri;
use function Facebook\FBExpect\expect;
use Waffle\Http\Message\UploadedFile;
use Waffle\Http\Message\Functional;
use Waffle\Contract\Http\Message\UploadedFileError;
use Facebook\HackTest\HackTest;

class ServerRequestTest extends HackTest
{
    public function testUploadedFiles()
    {
        $request1 = new ServerRequest('GET', new Uri('/'));

        $files = Map {
            'file' => new UploadedFile(
                Functional\CreateStreamFromString('test'), 
                123, 
                UploadedFileError::ERROR_OK
            ),
        };

        $request2 = $request1->withUploadedFiles($files);

        expect($request1)->toNotBeSame($request2);
        expect($request1->getUploadedFiles())->toBePHPEqual(Map {});
        expect($request2->getUploadedFiles())->toBeSame($files);
    }

    public function testServerParams()
    {
        $params = Map { 'name' => 'value' };

        $request = new ServerRequest('GET', new Uri('/'), Map {}, null, '1.1', $params);
        expect($request->getServerParams())->toBeSame($params);
    }

    public function testCookieParams()
    {
        $request1 = new ServerRequest('GET', new Uri ('/') );

        $params = Map { 'name' => 'value' };

        $request2 = $request1->withCookieParams($params);

        expect($request1)->toNotBeSame($request2);
        expect($request1->getCookieParams())->toBeEmpty();
        expect($request2->getCookieParams())->toBeSame($params);
    }

    public function testQueryParams()
    {
        $request1 = new ServerRequest('GET', new Uri ('/') );

        $params = Map { 'name' => 'value' };

        $request2 = $request1->withQueryParams($params);

        expect($request1)->toNotBeSame($request2);
        expect($request1->getQueryParams())->toBeEmpty();
        expect($request2->getQueryParams())->toBeSame($params);
    }

    public function testParsedBody()
    {
        $request1 = new ServerRequest('GET', new Uri('/'));

        $params = Map { 'name' => 'value' };

        $request2 = $request1->withParsedBody($params);

        expect($request1)->toNotBeSame($request2);
        expect($request1->getParsedBody())->toBeEmpty();
        expect($request2->getParsedBody())->toBeSame($params);
    }

    public function testAttributes()
    {
        $request1 = new ServerRequest('GET', new Uri ('/'));

        $request2 = $request1->withAttribute('name', 'value');
        $request3 = $request2->withAttribute('other', 'otherValue');
        $request4 = $request3->withoutAttribute('other');
        $request5 = $request3->withoutAttribute('unknown');

        expect($request1)->toNotBeSame($request2);
        expect($request2)->toNotBeSame($request3);
        expect($request3)->toNotBeSame($request4);
        expect($request4)->toNotBeSame($request5);

        expect($request1->getAttributes())->toBeEmpty();
        expect($request1->getAttribute('name'))->toBeEmpty();
        expect(            $request1->getAttribute('name', 'something'))->toBePHPEqual(
            'something',
            'Should return the default value'
        );

        expect($request2->getAttribute('name'))->toBePHPEqual('value');
        expect($request2->getAttributes())->toBePHPEqual(Map { 'name' => 'value' });
        expect($request3->getAttributes())->toBePHPEqual(Map { 'name' => 'value', 'other' => 'otherValue'});
        expect($request4->getAttributes())->toBePHPEqual(Map { 'name' => 'value' });
    }

    public function testNullAttribute()
    {
        $request = (new ServerRequest('GET', new Uri('/')))->withAttribute('name', null);

        expect($request->getAttributes())->toBePHPEqual(Map { 'name' => null });
        expect($request->getAttribute('name', 'different-default'))->toBeNull();

        $requestWithoutAttribute = $request->withoutAttribute('name');

        expect($requestWithoutAttribute->getAttributes())->toBePHPEqual(Map {});
        expect($requestWithoutAttribute->getAttribute('name', 'different-default'))->toBeSame('different-default');
    }
}
