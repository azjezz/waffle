<?hh // strict

namespace Waffle\Lib;

class Shape
{
    public function create<Tk as string, Tv>(KeyedContainer<Tk, Tv> $container = dict[]): shape(...)
    {
        $shape = shape();
        foreach ($container as $key => $value) {
            /* HH_IGNORE_ERROR[4051] */
            $shape[$key] = $value;
        }
        return $shape;
    }
}
