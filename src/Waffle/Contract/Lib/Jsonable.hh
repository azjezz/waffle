<?hh // strict

namespace Waffle\Contract\Lib;

interface Jsonable
{
    /**
     * Return a valid json string.
     */
    public function toJson(): string;
}