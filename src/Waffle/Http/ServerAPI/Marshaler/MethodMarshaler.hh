<?hh // strict

namespace Waffle\Http\ServerAPI\Marshaler;

class MethodMarshaler
{
    public function marshal(Map<string, mixed> $server): string
    {
        return $server->contains('REQUEST_METHOD') ? (string) $server->at('REQUEST_METHOD') : 'GET';
    }
}
