<?hh // strict

namespace Waffle\Cache;

use namespace HH\Lib\{C, Str};use type Waffle\Contract\Cache\CacheItemInterface;
use type DateTimeInterface;
use type DateInterval;
use type DateTime;

class CacheItem implements CacheItemInterface
{
    public function __construct(
        protected string $key,
        protected mixed $value = null,
        protected bool $isHit = false,
        protected ?int $expirationTimestamp = null,
    ) {}

    /**
     * Returns the key for the current cache item.
     */
    public function getKey(): string
    {
        return $this->key;
    }

    /**
     * Retrieves the value of the item from the cache associated with this object's key.
     */
    public function get(): mixed
    {
        return $this->value;
    }

    /**
     * Confirms if the cache item lookup resulted in a cache hit.
     */
    public function isHit(): bool
    {
        return $this->isHit;
    }

    /**
     * Sets the value represented by this cache item.
     */
    public function set(mixed $value): this
    {
        $this->value = $value;
        return $this;
    }

    public function expiresAt(?DateTimeInterface $expiration): this
    {
        $this->expirationTimestamp = $expiration is nonnull ? $expiration->getTimestamp() : null;

        return $this;
    }

    public function expiresAfter(?DateInterval $time): this
    {
        if (null === $time) {
            $this->expirationTimestamp = null;
        } else {
            $date = new DateTime('now');
            $date->add($time);
            $this->expirationTimestamp = $date->getTimestamp();
        }

        return $this;
    }

    /**
     * The timestamp when the object expires.
     */
    public function getExpirationTimestamp(): ?int
    {
        return $this->expirationTimestamp;
    }

    /**
     * Validates a cache key.
     *
     * @throws InvalidArgumentException When $key is not valid
     */
    public static function validateKey(string $key): string
    {
        if ('' === $key) {
            throw new Exception\InvalidArgumentException('Cache key length must be greater than zero');
        }

        if (false !== \strpbrk($key, '{}()/\@:')) {
            throw new Exception\InvalidArgumentException(Str\format('Cache key "%s" contains reserved characters {}()/\@:', $key));
        }

        return $key;
    }
}
