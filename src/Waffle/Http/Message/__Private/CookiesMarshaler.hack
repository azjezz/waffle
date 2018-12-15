<?hh // strict

namespace Waffle\Http\Message\__Private;

use function preg_match_all;
use function urldecode;
use const PREG_SET_ORDER;

class CookiesMarshaler
{
    public function marshal(vec<string> $header, KeyedContainer<string, string> $cookies): dict<string, string>
    {
        $dict = dict[];
        foreach ($cookies as $key => $value) {
            $dict[$key] = $value;
        }

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
            $dict[$match['name'] as string] = urldecode($match['value'] as string);
        }

        return $dict;
    }
}
