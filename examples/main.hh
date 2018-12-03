<?hh // strict

use namespace Waffle\Http\ServerApi;
use namespace Waffle\Http\Message\Response;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Message\CookieSameSite;
use type Waffle\Http\Message\Cookie;

require __DIR__.'/../vendor/hh_autoload.php';

class HelloWorldHandler implements RequestHandlerInterface
{
    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        $method = $request->getMethod();
        $target = $request->getRequestTarget();

        $response = new Response\HtmlResponse((string) (
            <html>
                <head>
                    <title>Hello, World!</title>
                </head>
                <body>
                    <h1>Hello, World!</h1>
                    <hr />
                    <code>{$method} {$target}</code>
                </body>
            </html>
        ));

        $response = $response->withCookie('name', new Cookie('value'));
        $response = $response->withCookie('no-send', new Cookie('value'));
        $response = $response->withoutCookie('no-send');
        return $response;
    }
}

<<__Entrypoint>>
function main(): noreturn
{
    // example handler ~
    $handler = new HelloWorldHandler();
    
    $marshaler = new ServerApi\Marshaler();
    $emitter = new ServerApi\Emitter();
    
    // marshal the request from SAPI 
    $request = $marshaler->marshal();
    // handle the request
    $response = $handler->handle($request);
    // emit the response
    $emitter->emit($response);
    // exit
    exit();
}