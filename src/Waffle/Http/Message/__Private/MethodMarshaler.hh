<?hh // strict

namespace Waffle\Http\Message\__Private;

use namespace HH\Lib\C;

class MethodMarshaler
{
    public function marshal(dict<string, mixed> $server): string
    {
        return C\contains($server, 'REQUEST_METHOD') ? (string) $server['REQUEST_METHOD'] : 'GET';
    }
}
