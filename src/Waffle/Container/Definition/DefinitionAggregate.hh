<?hh // strict

namespace Waffle\Container\Definition;

use Generator;
use Waffle\Container\ContainerAwareTrait;
use Waffle\Container\Exception\NotFoundException;
use function sprintf;
use Iterator;

class DefinitionAggregate implements DefinitionAggregateInterface
{
    use ContainerAwareTrait;

    public function __construct(
        protected Vector<DefinitionInterface> $definitions = new Vector<DefinitionInterface>([])
    ) {}

   /**
     * {@inheritdoc}
     */
    public function add(string $id, mixed $definition, bool $shared = false) : DefinitionInterface
    {
        if (! $definition instanceof DefinitionInterface) {
            $definition = (new Definition($id, $definition));
        }
        $this->definitions[] = $definition
            ->setAlias($id)
            ->setShared($shared)
        ;
        return $definition;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $id): bool
    {
        foreach ($this->getIterator() as $definition) {
            if ($id === $definition->getAlias()) {
                return true;
            }
        }

        return false;
    }

    /**
     * {@inheritdoc}
     */
    public function hasTag(string $tag): bool
    {
        foreach ($this->getIterator() as $definition) {
            if ($definition->hasTag($tag)) {
                return true;
            }
        }

        return false;
    }

    /**
     * {@inheritdoc}
     */
    public function getDefinition(string $id): DefinitionInterface
    {
        foreach ($this->getIterator() as $definition) {
            if ($id === $definition->getAlias()) {
                $definition->setContainer($this->getContainer());
                return $definition;
            }
        }

        throw new NotFoundException(sprintf('Alias (%s) is not being handled as a definition.', $id));
    }

    /**
     * {@inheritdoc}
     */
    public function resolve(string $id, bool $new = false): mixed
    {
        return $this->getDefinition($id)->resolve($new);
    }

    /**
     * {@inheritdoc}
     */
    public function resolveTagged(string $tag, bool $new = false): Vector<mixed>
    {
        $set = new Vector<mixed>([]);

        foreach ($this->getIterator() as $definition) {
            if ($definition->hasTag($tag)) {
                $set->add(
                    $definition->setContainer($this->getContainer())->resolve($new)
                );
            }
        }

        return $set;
    }

    public function getIterator(): Iterator<DefinitionInterface>
    {
        return $this->definitions->getIterator();
    }
}
