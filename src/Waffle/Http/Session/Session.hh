<?hh // strict

namespace Waffle\Http\Session;

use namespace HH\Lib\C;
use type Waffle\Contract\Http\Session\SessionInterface;

class Session implements SessionInterface
{
    /**
     * Current data within the session.
     */
    private dict<string, mixed> $data;

    private bool $isRegenerated = false;

    private dict<string, mixed> $originalData;

    /**
     * Lifetime of the session cookie.
     */
    private int $lifetime = 0;

    public function __construct(
        KeyedContainer<string, mixed> $data,
        private string $id = ''
    ) {
        $this->data = $this->originalData = dict($data);

        if (C\contains_key($data, SessionInterface::SESSION_LIFETIME_KEY)) {
            $this->lifetime = (int) $data[SessionInterface::SESSION_LIFETIME_KEY];
        }
    }

    /**
     * Retrieve the session identifier.
     */
    public function getId(): string
    {
        return $this->id;
    }

    /**
     * Retrieve a value from the session.
     *
     * @param mixed $default Default value to return if $name does not exist.
     */
    public function get(string $name, mixed $default = null): mixed
    {
        if ($this->contains($name)) {
            return $this->data[$name];
        } else {
            return $default;
        }
    }

    /**
     * Whether or not the container has the given key.
     */
    public function contains(string $name): bool
    {
        return C\contains_key($this->data, $name);
    }

    /**
     * Set a value within the session.
     *
     * Values MUST be serializable in any format; we recommend ensuring the
     * values are JSON serializable for greatest portability.
     */
    public function set(string $name, mixed $value): void
    {
        $this->data[$name] = $value;
    }

    /**
     * Remove a value from the session.
     */
    public function remove(string $name): void
    {
        unset($this->data[$name]);
    }

    /**
     * Clear all values.
     */
    public function clear(): void
    {
        $this->data = dict[];
    }

    /**
     * Does the session contain changes? If not, the middleware handling
     * session persistence may not need to do more work.
     */
    public function hasChanged(): bool
    {
        if ($this->isRegenerated()) {
            return true;
        }

        return $this->data !== $this->originalData;
    }

    /**
     * Regenerate the session.
     *
     * This can be done to prevent session fixation. When executed, it SHOULD
     * return a new instance; that instance should always return true for
     * isRegenerated().
     *
     * An example of where this WOULD NOT return a new instance is within the
     * shipped LazySession, where instead it would return itself, after
     * internally re-setting the proxied session.
     */
    public function regenerate(): SessionInterface
    {
        $session = clone $this;
        $session->isRegenerated = true;
        return $session;
    }

    /**
     * Method to determine if the session was regenerated; should return
     * true if the instance was produced via regenerate().
     */
    public function isRegenerated(): bool
    {
        return $this->isRegenerated;
    }

    /**
     * Define how long the session cookie should live.
     *
     * Use this value to detail to the session persistence engine how long the
     * session cookie should live.
     *
     * This value could be passed as the $lifetime value of
     * session_set_cookie_params(), or used to create an Expires or Max-Age
     * parameter for a session cookie.
     *
     * Since cookie lifetime is communicated by the server to the client, and
     * not vice versa, the value should likely be persisted in the session
     * itself, to ensure that session regeneration uses the same value. We
     * recommend using the SESSION_LIFETIME_KEY value to communicate this.
     *
     * @param int $duration Number of seconds the cookie should persist for.
     */
    public function persist(int $duration): void
    {
        $this->lifetime = $duration;
        $this->set(
            SessionInterface::SESSION_LIFETIME_KEY,
            $duration
        );
    }

    /**
     * Determine how long the session cookie should live.
     *
     * Generally, this will return the value provided to persistFor().
     *
     * If that method has not been called, the value can return one of the
     * following:
     *
     * - 0 or a negative value, to indicate the cookie should be treated as a
     *   session cookie, and expire when the window is closed. This should be
     *   the default behavior.
     * - If persist() was provided during session creation or anytime later,
     *   the persistence engine should pull the TTL value from the session itself
     *   and return it here. Typically, this value should be communicated via
     *   the SESSION_LIFETIME_KEY value of the session.
     */
    public function lifetime(): int
    {
        return $this->lifetime;
    }

    public function getContainer(): KeyedContainer<string, mixed>
    {
        return $this->data;
    }
}
