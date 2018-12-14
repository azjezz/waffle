<?hh

namespace Waffle\Tests\Container\Argument;

use Facebook\HackTest\HackTest;
use Waffle\Container\Argument\RawArgument;
use Waffle\Container\Exception\NotFoundException;
use Waffle\Container\Container;
use Waffle\Contract\Container\ContainerInterface;
use Waffle\Tests\Container\Asset\QuxArgumentResolver;
use Waffle\Tests\Container\Asset\Foo;
use ReflectionClass;
use ReflectionFunctionAbstract;
use ReflectionParameter;
use function Facebook\FBExpect\expect;

class ArgumentResolverTest extends HackTest
{
    /**
     * Asserts that the resolver proxies to container for resolution.
     */
    public function testResolverResolvesFromContainer()
    {
        $resolver = new QuxArgumentResolver();

        $container = new Container();

        $container->add('alias1', () ==> $resolver);

        $resolver->setContainer($container);

        $args = $resolver->resolveArguments(vec[
            'alias1', 'alias2'
        ]);

        expect($args[0])->toBeSame($resolver);
        expect($args[1])->toBeSame('alias2');
    }

    /**
     * Asserts that the resolver resolves raw arguments.
     */
    public function testResolverResolvesResolvesRawArguments()
    {
        $resolver = new QuxArgumentResolver();

        $container = new Container();

        $container->add('alias1', () ==> new RawArgument('value1'));

        $resolver->setContainer($container);

        $args = $resolver->resolveArguments(vec[
            'alias1', new RawArgument('value2')
        ]);

        expect($args[0])->toBeSame('value1');
        expect($args[1])->toBeSame('value2');
    }

    /**
     * Asserts that the resolver can resolve arguments via reflection.
     */
    public function testResolverResolvesArgumentsViaReflection()
    {
        $llama = (Foo $foo, string $param2, string $param3 = 'default'): string 
                  ==> 'i am a ' . $param2;

        $method = new \ReflectionFunction($llama);
        $container = new Container();

        $resolver = new QuxArgumentResolver();

        $resolver->setContainer($container);

        $foo = new Foo(null);

        /*------------------------------------------------*/

        $args = $resolver->reflectArguments($method, dict[
            'param2' => 'llama',
            'foo' => $foo
        ]);

        expect($args[0])->toBeSame($foo);
        expect($args[1])->toBeSame('llama');
        expect($args[2])->toBeSame('default');

        /*------------------------------------------------*/

        $args = $resolver->reflectArguments($method, dict[
            'param2' => 'llama'
        ]);

        expect($args[0])->toBeSame(Foo::class);
        expect($args[1])->toBeSame('llama');
        expect($args[2])->toBeSame('default');
    }

    /**
     * Asserts that the resolver throws an exception when reflection can't resolve a value.
     */
    public function testResolverThrowsExceptionWhenReflectionDoesNotResolve()
    {
        expect(() ==> {

            $func = ($param1): string ==> (string) $param1;

            $method = new \ReflectionFunction($func);

            $resolver = new QuxArgumentResolver();

            $args = $resolver->reflectArguments($method);

        })->toThrow(NotFoundException::class);
    }
}
