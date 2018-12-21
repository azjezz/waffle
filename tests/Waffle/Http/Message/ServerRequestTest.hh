<?hh

namespace Waffle\Tests\Http\Message;

use type Waffle\Http\Message\ServerRequest;
use type Waffle\Http\Message\Uri;
use function Facebook\FBExpect\expect;
use type Waffle\Http\Message\UploadedFile;
use type Waffle\Http\Message\UploadsFolder;
use namespace Waffle\Http\Message\__Private;
use type Waffle\Contract\Http\Message\UploadedFileError;
use type Facebook\HackTest\HackTest;

class ServerRequestTest extends HackTest
{
    public function testUploadsFolder()
    {
        $request1 = new ServerRequest('GET', new Uri('/'));

        $file = new UploadedFile(
            __Private\create_stream_from_string('test'),
            123,
            UploadedFileError::ERROR_OK
        );

        $folder = (new UploadsFolder())->withFile('file', $file);

        $request2 = $request1->withUploadsFolder($folder);

        expect($request1)->toNotBeSame($request2);
        expect($request1->getUploadsFolder())->toBeNull();
        expect($request2->getUploadsFolder())->toBeSame($folder);
        expect($request2->getUploadsFolder()?->getFile('file'))->toBeSame($file);
    }

    public function testServerParams()
    {
        $params = dict[ 'name' => 'value' ];

        $request = new ServerRequest('GET', new Uri('/'), dict[], null, '1.1', $params);
        expect($request->getServerParams())->toBeSame($params);
    }

    public function testCookieParams()
    {
        $request1 = new ServerRequest('GET', new Uri ('/') );

        $params = dict['name' => 'value'];

        $request2 = $request1->withCookieParams($params);

        expect($request1)->toNotBeSame($request2);
        expect($request1->getCookieParams())->toBeEmpty();
        expect($request2->getCookieParams())->toBeSame($params);
    }

    public function testQueryParams()
    {
        $request1 = new ServerRequest('GET', new Uri ('/') );

        $params = dict[ 'name' => 'value' ];

        $request2 = $request1->withQueryParams($params);

        expect($request1)->toNotBeSame($request2);
        expect($request1->getQueryParams())->toBeEmpty();
        expect($request2->getQueryParams())->toBeSame($params);
    }

    public function testParsedBody()
    {
        $request1 = new ServerRequest('GET', new Uri('/'));

        $params = dict[ 'name' => 'value' ];

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
        expect($request1->getAttribute('name'))->toBeNull();
        expect($request1->getAttribute('name', 'something'))->toBeSame(
            'something',
            'Should return the default value'
        );

        expect($request2->getAttribute('name'))->toBeSame('value');
        expect($request2->getAttributes())->toBeSame(dict[ 'name' => 'value' ]);
        expect($request3->getAttributes())->toBeSame(dict[ 'name' => 'value', 'other' => 'otherValue']);
        expect($request4->getAttributes())->toBeSame(dict[ 'name' => 'value' ]);
    }

    public function testNullAttribute()
    {
        $request = (new ServerRequest('GET', new Uri('/')))->withAttribute('name', null);

        expect($request->getAttributes())->toBeSame(dict['name' => null]);
        expect($request->getAttribute('name', 'different-default'))->toBeNull();

        $requestWithoutAttribute = $request->withoutAttribute('name');

        expect($requestWithoutAttribute->getAttributes())->toBeSame(dict[]);
        expect($requestWithoutAttribute->getAttribute('name', 'different-default'))->toBeSame('different-default');
    }
}
