<?hh // strict

namespace Waffle\Config\__Private;


function merge_recursive<Tk as arraykey, Tv>(
  KeyedTraversable<Tk, Tv> $first,
  KeyedTraversable<Tk, Tv> ...$rest
): dict<Tk, Tv> {
    $first = dict($first);

    foreach ($rest as $other) {
        foreach ($other as $key => $value) {
            $fValue = $first[$key] ?? null;
            if ($value is KeyedTraversable<_, _> && $fValue is KeyedTraversable<_, _>) {
                /* HH_IGNORE_ERROR[4110] */
                $first[$key] = merge_recursive($fValue, $value);
            } else {
                $first[$key] = $value;
            }
        }
    }
    /* HH_IGNORE_ERROR[4110] */
    return $first;
}
