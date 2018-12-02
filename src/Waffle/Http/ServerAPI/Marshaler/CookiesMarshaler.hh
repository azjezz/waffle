<?hh // strict

namespace Waffle\Http\ServerAPI\Marshaler;

use function preg_match_all;
use function urldecode;
use const PREG_SET_ORDER;

class CookiesMarshaler
{
    public function marshal(Set<string> $header, array<string, string> $cookies): Map<string, string>
    {
        $cookies = new Map<string, string>($cookies);

        $matches = [];

        preg_match_all('(
            (?:^\\n?[ \t]*|;[ ])
            (?P<name>[!#$%&\'*+-.0-9A-Z^_`a-z|~]+)
            =
            (?P<DQUOTE>"?)
                (?P<value>[\x21\x23-\x2b\x2d-\x3a\x3c-\x5b\x5d-\x7e]*)
            (?P=DQUOTE)
            (?=\\n?[ \t]*$|;[ ])
        )x', $header, &$matches, PREG_SET_ORDER);

        foreach ($matches as $match) {
            $cookies[$match['name']] = urldecode($match['value']);
        }

        return $cookies;
    }
}
