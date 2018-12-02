<?hh // strict

namespace Waffle\Http\ServerAPI;

use type Waffle\Http\Message\ServerRequest;
use type Waffle\Http\Message\Stream;
use function fopen;

class Marshaler
{
    public function marshalServerRequest(): ServerRequest
    {
        /* HH_IGNORE_ERROR[2050] */
        $server = (new Marshaler\ServerParametersMarshaler())->marshale($_SERVER);
        $headers = (new Marshaler\HeadersMarshaler())->marshal($server);
        /* HH_IGNORE_ERROR[2050] */
        $cookies = (new Marshaler\CookiesMarshaler())->marshal($headers->get('cookie') ?? Set {}, $_COOKIE);
        $uri = (new Marshaler\UriMarshaler())->marshal($server, $headers);
        /* HH_IGNORE_ERROR[2050] */
        $uploads = (new Marshaler\UploadsFolderMarshaler())->marshal($_FILES);
        $method = (new Marshaler\MethodMarshaler())->marshal($server);
        /* HH_IGNORE_ERROR[2050] */
        $body = new Map<string, mixed>($_POST);
        /* HH_IGNORE_ERROR[2050] */
        $query = new Map<string, mixed>($_GET);
        $protocolVersion = (new Marshaler\ProtocolVersionMarshaler())->marshal($server);

        $stream = new Stream(fopen('php://input', 'r'));

        return (new ServerRequest($method, $uri, $headers, $stream, $protocolVersion, $server))
            ->withCookieParams($cookies)
            ->withQueryParams($query)
            ->withParsedBody($body)
            ->withUploadsFolder($uploads);
    }
}
