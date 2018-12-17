<?hh // strict

namespace Waffle\Contract\Cache;

interface TaggableCacheItemPoolInterface extends CacheItemPoolInterface
{
    /**
     * Invalidates cached items using a tag.
     *
     * @param string $tag The tag to invalidate
     *
     * @throws InvalidArgumentException When $tags is not valid
     *
     * @return bool True on success
     */
    public function invalidateTag(string $tag): bool;

    /**
     * Invalidates cached items using tags.
     *
     * @return bool True on success, False otherwise.
     */
    public function invalidateTags(vec<string> $tags): bool;
}
