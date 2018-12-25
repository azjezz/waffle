<?hh // strict

namespace Example\App\Handler;


use namespace Waffle\Http\Message;
use type Waffle\Contract\Http\Server\RequestHandlerInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;

class HomeHandler implements RequestHandlerInterface
{
    public function handle(ServerRequestInterface $request): ResponseInterface
    {
        $html = <<<HTML
<html>
    <head>
        <title>Waffle Example App</title>
    </head>
    <body>
        <h1>Welcome to Waffle</h1>
        <hr />
        <a href='/ping'>Ping me!</a><br/>
        <a href='/data'>Get some data !</a>
    </body>
</html>
HTML;
        return new Message\Response\HtmlResponse($html, Message\StatusCode::STATUS_OK)
            |> $$->withCookie('name', new Message\Cookie('value'))
            |> $$->withCookie('another-name', new Message\Cookie('another-value'))
            |> $$->withAddedHeader('Tk', vec(['N']));
    }
}
