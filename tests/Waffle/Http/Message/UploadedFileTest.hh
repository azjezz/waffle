<?hh

namespace Waffle\Tests\Http\Message;

use Waffle\Http\Message\Stream;
use Waffle\Http\Message\Exception;
use Waffle\Http\Message\UploadedFile;
use Waffle\Contract\Http\Message\UploadedFileError;
use Facebook\HackTest\HackTest;
use function Facebook\FBExpect\expect;
use function Waffle\Http\Message\Functional\create_stream_from_string;
use function is_scalar;
use function file_exists;
use function unlink;
use function fopen;
use function tempnam;
use function sys_get_temp_dir;
use function uniqid;
use function file_get_contents;

class UploadedFileTest extends HackTest
{
    protected array $cleanup = [];

    public async function beforeEachTestAsync(): Awaitable<void>
    {
        $this->cleanup = [];
    }

    public async function afterEachTestAsync(): Awaitable<void>
    {
        foreach ($this->cleanup as $file) {
            if (is_scalar($file) && file_exists($file)) {
                unlink($file);
            }
        }
    }

    public function testGetStreamReturnsOriginalStreamObject()
    {
        $stream = create_stream_from_string('');
        $upload = new UploadedFile($stream, 0, UploadedFileError::ERROR_OK);

        expect($upload->getStream())->toBeSame($stream);
    }

    public function testGetStreamReturnsWrappedPhpStream()
    {
        $handle = fopen('php://temp', 'wb+');
        $upload = new UploadedFile(new Stream($handle), 0, UploadedFileError::ERROR_OK);
        $uploadHandle = $upload->getStream()->detach();

        expect($uploadHandle)->toBeSame($handle);
    }

    public function testSuccessful()
    {
        $stream = create_stream_from_string('Foo bar!');
        $upload = new UploadedFile($stream, $stream->getSize(), UploadedFileError::ERROR_OK, 'filename.txt', 'text/plain');

        expect($upload->getSize())->toBePHPEqual($stream->getSize());
        expect($upload->getClientFilename())->toBePHPEqual('filename.txt');
        expect($upload->getClientMediaType())->toBePHPEqual('text/plain');

        $this->cleanup[] = $to = tempnam(sys_get_temp_dir(), 'successful');
        $upload->moveTo($to);
        expect(file_get_contents($to))->toBePHPEqual($stream->__toString());
    }

    public function testMoveCannotBeCalledMoreThanOnce()
    {
        $stream = create_stream_from_string('Foo bar!');
        $upload = new UploadedFile($stream, 0, UploadedFileError::ERROR_OK);

        $this->cleanup[] = $to = tempnam(sys_get_temp_dir(), 'diac');
        $upload->moveTo($to);
        expect(file_exists($to))->toBeTrue();
        expect(() ==> {
            $upload->moveTo($to);
        })->toThrow(
            Exception\UploadedFileAlreadyMovedException::class,
            'Cannot retrieve stream after it has already moved'
        );
    }

    public function testCannotRetrieveStreamAfterMove()
    {
        $stream = create_stream_from_string('Foo bar!');
        $upload = new UploadedFile($stream, 0, UploadedFileError::ERROR_OK);

        $this->cleanup[] = $to = tempnam(sys_get_temp_dir(), 'diac');
        $upload->moveTo($to);
        expect(() ==> {
            $upload->getStream();
        })->toThrow(
            Exception\UploadedFileAlreadyMovedException::class,
            'Cannot retrieve stream after it has already moved'
        );
    }

    public function nonOkErrorStatus()
    {
        return [
            'UPLOAD_ERR_INI_SIZE'   => [UploadedFileError::ERROR_EXCEEDS_MAX_INI_SIZE],
            'UPLOAD_ERR_FORM_SIZE'  => [UploadedFileError::ERROR_EXCEEDS_MAX_FORM_SIZE],
            'UPLOAD_ERR_PARTIAL'    => [UploadedFileError::ERROR_INCOMPLETE],
            'UPLOAD_ERR_NO_FILE'    => [UploadedFileError::ERROR_NO_FILE],
            'UPLOAD_ERR_NO_TMP_DIR' => [UploadedFileError::ERROR_TMP_DIR_NOT_SPECIFIED],
            'UPLOAD_ERR_CANT_WRITE' => [UploadedFileError::ERROR_TMP_DIR_NOT_WRITEABLE],
            'UPLOAD_ERR_EXTENSION'  => [UploadedFileError::ERROR_CANCELED_BY_EXTENSION],
        ];
    }

    <<DataProvider('nonOkErrorStatus')>>
    public function testConstructorDoesNotRaiseExceptionForInvalidStreamWhenErrorStatusPresent($status)
    {
        $uploadedFile = new UploadedFile(create_stream_from_string(''), 0, $status);
        expect($uploadedFile->getError())->toBeSame($status);
    }

    <<DataProvider('nonOkErrorStatus')>>
    public function testMoveToRaisesExceptionWhenErrorStatusPresent($status)
    {
        $uploadedFile = new UploadedFile(create_stream_from_string(''), 0, $status);
        expect(() ==> {
            $uploadedFile->moveTo(__DIR__.'/'.uniqid());
        })->toThrow(
            Exception\UploadedFileErrorException::class,
            'Cannot retrieve stream due to upload error'
        );
    }

    <<DataProvider('nonOkErrorStatus')>>
    public function testGetStreamRaisesExceptionWhenErrorStatusPresent($status)
    {
        $uploadedFile = new UploadedFile(create_stream_from_string(''), 0, $status);
        expect(() ==> {
            $stream = $uploadedFile->getStream();
        })->toThrow(
            Exception\UploadedFileErrorException::class,
            'Cannot retrieve stream due to upload error'
        );
    }
}
