<?hh // strict

namespace Waffle\Http\Session;

use type Waffle\Contract\Http\Session\SessionInterface;
use type Waffle\Contract\Http\Message\ServerRequestInterface;

final class LazySession implements SessionInterface
{
    private ?SessionInterface $proxiedSession;

    public function __construct(
        private Persistence\SessionPersistenceInterface $persistence,
        private ServerRequestInterface $request
    ) {}

    public function getId(): string
    {
        return $this->getProxiedSession()->getId();
    }

    public function regenerate(): SessionInterface
    {
        $this->proxiedSession = $this->getProxiedSession()->regenerate();
        return $this;
    }

    public function isRegenerated(): bool
    {
        return $this->getProxiedSession()->isRegenerated();
    }


    public function get(string $name, mixed $default = null): mixed
    {
        return $this->getProxiedSession()->get($name, $default);
    }

    public function contains(string $name): bool
    {
        return $this->getProxiedSession()->contains($name);
    }

    public function set(string $name, mixed $value): void
    {
        $this->getProxiedSession()->set($name, $value);
    }

    public function remove(string $name): void
    {
        $this->getProxiedSession()->remove($name);
    }

    public function clear(): void
    {
        $this->getProxiedSession()->clear();
    }

    public function hasChanged(): bool
    {
        if (null === $this->proxiedSession) {
            return false;
        }

        $session = $this->getProxiedSession();

        if ($session->isRegenerated()) {
            return true;
        }

        return $session->hasChanged();
    }

    public function persist(int $duration): void
    {
        $this->getProxiedSession()->persist($duration);
    }

    public function lifetime(): int
    {
        return $this->getProxiedSession()->lifetime();
    }

    /**
     * Return data which can be serialized with json_encode.
     */
    public function getContainer(): KeyedContainer<string, mixed>
    {
        return $this->getProxiedSession()->getContainer();
    }

    private function getProxiedSession(): SessionInterface
    {
        if ($this->proxiedSession is nonnull) {
            return $this->proxiedSession;
        }

        $this->proxiedSession = $this->persistence->initialize($this->request);
        return $this->proxiedSession;
    }
}
