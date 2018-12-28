<?hh // strict

namespace Waffle\Http\Session\Persistence;

use namespace HH\Lib\C;
use namespace HH\Lib\Str;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Session\SessionInterface;
use type Waffle\Contract\Cache\CacheItemPoolInterface;
use type Waffle\Http\Session\Session;
use type Waffle\Http\Session\SessionOptions;
use type DateInterval;

/**
 * Session persistence using a cache item pool.
 *
 * Session identifiers are generated using random_bytes (and casting to hex).
 * During persistence, if the session regeneration flag is true, a new session
 * identifier is created, and the session re-started.
 */
class CacheSessionPersistence extends AbstractSessionPersistence
{
    public function __construct(
        private CacheItemPoolInterface $cache,
        protected SessionOptions $options
    ) {}

    <<__Override>>
    public function initialize(ServerRequestInterface $request): SessionInterface
    {
        $this->pathTranslated = (string) ( $request->getServerParams()['PATH_TRANSLATED'] ?? '' );
        $id = $this->getCookieFromRequest($request);
        $sessionData = $id ? $this->getSessionDataFromCache($id) : dict[];
        $session = new Session($sessionData, $id);
        $session->expire($this->options['cookie']['lifetime'] ?? 0);
        return $session;
    }

    <<__Override>>
    public function persist(SessionInterface $session, ResponseInterface $response): ResponseInterface
    {
        $id = $session->getId();

        if ($session->flushed()) {
            if (!Str\is_empty($id)) {
                $this->cache->hasItem($id) && $this->cache->deleteItem($id);
            }

            return $this->flush($session, $response);
        }

        // New session? No data? Nothing to do.
        if (Str\is_empty($id) && (C\is_empty($session->items()) || !$session->changed())) {
            return $response;
        }

        // Regenerate the session if:
        // - we have no session identifier
        // - the session is marked as regenerated
        // - the session has changed (data is different)
        if (Str\is_empty($id) || $session->regenerated() || $session->changed()) {
            $id = $this->regenerateSession($id);
        }

        $this->persistSessionDataToCache($id, $session->items(), $session->age());

        return $this->withCacheHeaders(
            $response->withCookie(
                $this->options['cookie']['name'] ?? 'hh-session',
                $this->createCookie($id, $session->age())
            )
        );
    }

    /**
     * Regenerates the session.
     *
     * If the cache has an entry corresponding to `$id`, this deletes it.
     *
     * Regardless, it generates and returns a new session identifier.
     */
    private function regenerateSession(string $id): string
    {
        if (!Str\is_empty($id) && $this->cache->hasItem($id)) {
            $this->cache->deleteItem($id);
        }
        return $this->generateSessionId();
    }

    private function getSessionDataFromCache(string $id): KeyedContainer<string, mixed>
    {
        $item = $this->cache->getItem($id);
        if (! $item->isHit()) {
            return dict[];
        }
        /* HH_IGNORE_ERROR[4110] */
        return $item->get() ?? dict[];
    }

    private function persistSessionDataToCache(string $id, KeyedContainer<string, mixed> $data, int $age): void
    {
        $item = $this->cache->getItem($id);
        $item->set($data);
        $item->expiresAfter(new DateInterval(Str\format('PT%dS', $age)));
        $this->cache->save($item);
    }
}
