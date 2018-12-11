<?hh // strict

namespace Waffle\Container\Inflector;

use type Waffle\Container\ContainerAwareTrait;
use type Iterator;
use function is_a;

class InflectorAggregate implements InflectorAggregateInterface
{
    use ContainerAwareTrait;

    public function __construct(
        protected Vector<Inflector> $inflectors = Vector {}
    ) {}

    /**
     * {@inheritdoc}
     */
    public function add(string $type, ?(function(mixed...): void) $callback = null): Inflector
    {
        $inflector = new Inflector($type, $callback);

        $this->inflectors->add($inflector);

        return $inflector;
    }

    /**
     * {@inheritdoc}
     */
    public function getIterator(): Iterator<Inflector>
    {
        return $this->inflectors->getIterator();
    }

    /**
     * {@inheritdoc}
     */
    public function inflect(mixed $object): mixed
    {
        foreach ($this->getIterator() as $inflector) {
            $type = $inflector->getType();

            if (!is_a($object, $type)) {
                continue;
            }

            $inflector->setContainer($this->getContainer());
            $inflector->inflect($object);
        }

        return $object;
    }
}
