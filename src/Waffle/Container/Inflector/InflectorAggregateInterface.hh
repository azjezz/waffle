<?hh // strict

namespace Waffle\Container\Inflector;

use type IteratorAggregate;
use type Waffle\Container\ContainerAwareInterface;

interface InflectorAggregateInterface extends ContainerAwareInterface, IteratorAggregate<Inflector>
{
    /**
     * Add an inflector to the aggregate.
     *
     * @param  callable $callback
     */
    public function add(string $type, mixed $callback = null): Inflector;

    /**
     * Applies all inflectors to an object.
     *
     * @param  object $object
     * @return object
     */
    public function inflect(mixed $object): mixed;
}
