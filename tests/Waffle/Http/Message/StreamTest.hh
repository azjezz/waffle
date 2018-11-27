<?hh

namespace Waffle\Tests\Http\Message;

use Waffle\Http\Message\Stream;
use function Facebook\FBExpect\expect;
use Facebook\HackTest\HackTest;

class StreamTest extends HackTest
{
    public function testConstructorInitializesProperties()
    {
        $handle = fopen('php://temp', 'r+');
        fwrite($handle, 'data');
        $stream = new Stream($handle);
        expect($stream->isReadable())->toBeTrue();
        expect($stream->isWritable())->toBeTrue();
        expect($stream->isSeekable())->toBeTrue();
        expect($stream->getMetadata('uri'))->toBeSame('php://temp');
        expect($stream->getSize())->toBeSame(4);
        expect($stream->eof())->toBeFalse();
        $stream->close();
    }

    public function testConvertsToString()
    {
        $handle = fopen('php://temp', 'w+');
        fwrite($handle, 'data');
        $stream = new Stream($handle);
        expect((string) $stream)->toBeSame('data');
        expect((string) $stream)->toBeSame('data');
        $stream->close();
    }

    public function testGetsContents()
    {
        $handle = fopen('php://temp', 'w+');
        fwrite($handle, 'data');
        $stream = new Stream($handle);
        expect($stream->getContents())->toBeSame('');
        $stream->seek(0);
        expect($stream->getContents())->toBeSame('data');
        expect($stream->getContents())->toBeSame('');
    }

    public function testChecksEof()
    {
        $handle = fopen('php://temp', 'w+');
        fwrite($handle, 'data');
        $stream = new Stream($handle);
        expect($stream->eof())->toBeFalse();
        $stream->read(4);
        expect($stream->eof())->toBeTrue();
        $stream->close();
    }

    public function testGetSize()
    {
        $size = filesize(__FILE__);
        $handle = fopen(__FILE__, 'r');
        $stream = new Stream($handle);
        expect($stream->getSize())->toBeSame($size);
        // Load from cache
        expect($stream->getSize())->toBeSame($size);
        $stream->close();
    }

    public function testEnsuresSizeIsConsistent()
    {
        $h = fopen('php://temp', 'w+');
        expect(fwrite($h, 'foo'))->toBeSame(3);
        $stream = new Stream($h);
        expect($stream->getSize())->toBeSame(3);
        expect($stream->write('test'))->toBeSame(4);
        expect($stream->getSize())->toBeSame(7);
        expect($stream->getSize())->toBeSame(7);
        $stream->close();
    }

    public function testProvidesStreamPosition()
    {
        $handle = fopen('php://temp', 'w+');
        $stream = new Stream($handle);
        expect($stream->tell())->toBeSame(0);
        $stream->write('foo');
        expect($stream->tell())->toBeSame(3);
        $stream->seek(1);
        expect($stream->tell())->toBeSame(1);
        expect($stream->tell())->toBeSame(ftell($handle));
        $stream->close();
    }

    public function testCanDetachStream()
    {
        $r = fopen('php://temp', 'w+');
        $stream = new Stream($r);
        $stream->write('foo');
        expect($stream->isReadable())->toBeTrue();
        expect($stream->detach())->toBeSame($r);
        $stream->detach();

        expect($stream->isReadable())->toBeFalse();
        expect($stream->isWritable())->toBeFalse();
        expect($stream->isSeekable())->toBeFalse();
        expect((string) $stream)->toBeSame('');
        $stream->close();
    }

    public function testCloseClearProperties()
    {
        $handle = fopen('php://temp', 'r+');
        $stream = new Stream($handle);
        $stream->close();

        expect($stream->isSeekable())->toBeFalse();
        expect($stream->isReadable())->toBeFalse();
        expect($stream->isWritable())->toBeFalse();
        expect($stream->getSize())->toBeNull();
        expect($stream->getMetadata())->toBeEmpty();
    }
}
