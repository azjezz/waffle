<?hh // strict

namespace Waffle\Http\ServerApi\Marshaler;

use namespace HH\Lib\Str;
use namespace HH\Lib\C;
use function strtr;
use function is_array;

class HeadersMarshaler
{
    public function marshal(Map<string, mixed> $server): Map<string, Set<string>>
    {
        $headers = Map {};

        $valid = (mixed $value): bool ==> {
            return is_array($value) ? C\count($value) > 0 : ((string) $value) !== '';
        };

        foreach ($server as $key => $value) {
            // Apache prefixes environment variables with REDIRECT_
            // if they are added by rewrite rules
            if (Str\search($key, 'REDIRECT_') === 0) {
                $key = Str\slice($key, 9);
                // We will not overwrite existing variables with the
                // prefixed versions, though
                if ($server->contains($key)) {
                    continue;
                }
            }

            if ($valid($value) && Str\search($key, 'HTTP_') === 0) {
                $name = strtr(Str\lowercase(Str\slice($key, 5)), '_', '-');
                if (!is_array($value)) {
                    $value = [(string) $value];
                }
                $value = new Set<string>($value);
                $headers->set($name, $value);
                continue;
            }

            if ($valid($value) && Str\search($key, 'CONTENT_') === 0) {
                $name = 'content-' . Str\lowercase(Str\slice($key, 8));
                if (!is_array($value)) {
                    $value = [(string) $value];
                }
                $value = new Set<string>($value);
                $headers->set($name, $value);
                continue;
            }
        }

        return $headers;
    }
}
