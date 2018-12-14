<?hh // strict

namespace Waffle\Http\Message\__Private;

use namespace HH\Lib\C;
use namespace Waffle\Http\Message\Exception;
use function preg_match;

class ProtocolVersionMarshaler
{
    public function marshal(dict<string, mixed> $server): string
    {
        if (!C\contains($server, 'SERVER_PROTOCOL')) {
            return '1.1';
        }

        $matches = [];

        if (! preg_match('#^(HTTP/)?(?P<version>[1-9]\d*(?:\.\d)?)$#',(string) $server['SERVER_PROTOCOL'], &$matches)) {
            throw Exception\UnrecognizedProtocolVersionException::forVersion(
                (string) $server['SERVER_PROTOCOL']
            );
        }

        return $matches['version'];
    }
}
