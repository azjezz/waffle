<?hh // strict

namespace Waffle\Http\ServerAPI\Marshaler;

use Waffle\Http\Message\Exception;
use function preg_match;

class ProtocolVersionMarshaler
{
    public function marshal(Map<string, mixed> $server): string
    {
        if (!$server->contains('SERVER_PROTOCOL')) {
            return '1.1';
        }

        $matches = [];

        if (! preg_match('#^(HTTP/)?(?P<version>[1-9]\d*(?:\.\d)?)$#',(string) $server->at('SERVER_PROTOCOL'), &$matches)) {
            throw Exception\UnrecognizedProtocolVersionException::forVersion(
                (string) $server->at('SERVER_PROTOCOL')
            );
        }

        return $matches['version'];
    }
}