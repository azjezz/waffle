<?hh // strict

namespace Waffle\Http\ServerAPI\Marshaler;

use function function_exists;

class ServerParametersMarshaler
{
    public function marshale(array<string, mixed> $server): Map<string, mixed>
    {
        return $this->normalize(
            new Map<string, mixed>($server)
        );
    }

    private function normalize(Map<string, mixed> $server): Map<string, mixed>
    {
        if ($server->contains('HTTP_AUTHORIZATION')) {
            return $server;
        }

        if (function_exists('apache_request_headers')) {
            $apacheRequestHeaders = new Map<string, string>((fun('apache_request_headers'))());

            if ($apacheRequestHeaders->contains('authorization')) {
                $server->set('HTTP_AUTHORIZATION', $apacheRequestHeaders->at('authorization'));
            }

            if ($apacheRequestHeaders->contains('Authorization')) {
                $server->set('HTTP_AUTHORIZATION', $apacheRequestHeaders->at('Authorization'));
            }

            if ($apacheRequestHeaders->contains('AUTHORIZATION')) {
                $server->set('HTTP_AUTHORIZATION', $apacheRequestHeaders->at('AUTHORIZATION'));
            }
        }

        return $server;
    }
}
