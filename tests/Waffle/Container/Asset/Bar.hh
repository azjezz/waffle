<?hh

namespace Waffle\Tests\Container\Asset;

class Bar
{
    protected $something;

    public function setSomething($something)
    {
        $this->something = $something;
    }
}
