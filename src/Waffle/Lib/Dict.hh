<?hh // strict

namespace Waffle\Lib;

use namespace HH\Lib\C;
use namespace HH\Lib\Vec;

class Dict
{
    public static function union<Tk as arraykey, Tv>(
        KeyedContainer<Tk, Tv> ...$containers
    ): dict<Tk, Tv> {
        $result = dict[];
        foreach ($containers as $container) {
            foreach ($container as $key => $value) {
                if (!C\contains_key($result, $key)) {
                    $result[$key] = $value;
                }
            }
        }
        return $result;
    }

    public static function replace<Tk as arraykey, Tv>(
        KeyedContainer<Tk, Tv> $container,
        KeyedContainer<Tk, Tv> ...$replacments
    ): dict<Tk, Tv> {
        $containers = Vec\reverse($replacments);
        $containers[] = $container;
        return static::union(...$containers);
    }
}
