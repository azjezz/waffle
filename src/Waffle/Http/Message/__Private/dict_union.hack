<?hh // strict

namespace Waffle\Http\Message\__Private;

use namespace HH\Lib\C;

function dict_union<Tk as arraykey, Tv>(
  KeyedTraversable<Tk, Tv> ...$traversables
): dict<Tk, Tv> {
  $result = dict[];
  foreach ($traversables as $traversable) {
    foreach ($traversable as $key => $value) {
        if (!C\contains_key($result, $key)) {
            $result[$key] = $value;
        }
    }
  }
  return $result;
}
