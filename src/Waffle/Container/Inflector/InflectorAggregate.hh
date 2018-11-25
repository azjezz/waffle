<?hh // strict

namespace Waffle\Container\Inflector;

use Generator;
use Waffle\Container\ContainerAwareTrait;
use function count;
use Iterator;
use function is_a;

class InflectorAggregate implements InflectorAggregateInterface
{
    use ContainerAwareTrait;

    public function __construct(
        protected Vector<Inflector> $inflectors = new Vector<Inflector>([])
    ) {}

    /**
     * {@inheritdoc}
     */
    public function add(string $type, mixed $callback = null): Inflector
    {
        $this->inflectors->add(
            $inflector = new Inflector($type, $callback)
        );

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
