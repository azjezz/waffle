<?hh

namespace Waffle\Tests\Container\Asset;

class Foo
{
    public ?Bar $bar;
    public static $staticBar;

    public static $staticHello;

    public function __construct(?Bar $bar) 
    {
        $this->bar = $bar;
    }

    public function setBar(Bar $bar)
    {
        $this->bar = $bar;
    }

    public static function staticSetBar(Bar $bar, $hello = 'hello world')
    {
        self::$staticHello = $hello;
        self::$staticBar = $bar;
    }
}
