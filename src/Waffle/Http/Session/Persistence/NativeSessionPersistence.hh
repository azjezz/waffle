<?hh // strict

namespace Waffle\Http\Session\Persistence;

use namespace HH\Lib\C;
use namespace HH\Lib\Str;
use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Session\SessionInterface;
use type Waffle\Http\Session\SessionOptions;
use type Waffle\Http\Session\Session;
use type SessionHandlerInterface;
use type SessionHandler;
use function bin2hex;
use function filemtime;
use function gmdate;
use function ini_get;
use function is_file;
use function random_bytes;
use function session_id;
use function session_name;
use function session_start;
use function session_commit;
use function sprintf;
use function time;
use function ini_set;

class NativeSessionPersistence extends AbstractSessionPersistence
{
    public function __construct(
        protected SessionOptions $options,
    ) {}

    /**
     * Generate a session data instance based on the request.
     */
    public function initialize(ServerRequestInterface $request): SessionInterface
    {

        $this->pathTranslated = (string) $request->getServerParams()['PATH_TRANSLATED'] ?? '';
        $sessionId = $this->getCookieFromRequest($request);
        $id = $sessionId === '' ? $this->generateSessionId() : $sessionId;
        $this->startSession($id);
        $session = new Session($_SESSION, $sessionId);
        $session->expire($this->options['cookie']['lifetime'] ?? 0);
        return $session;
    }

    /**
     * Persist the session data instance
     *
     * Persists the session data, returning a response instance with any
     * artifacts required to return to the client.
     */
    public function persist(SessionInterface $session, ResponseInterface $response): ResponseInterface
    {
        if ($session->flushed()) {
            return $this->flush($session, $response);
        }

        $id = $session->getId();

        // New session? No data? Nothing to do.
        if (Str\is_empty($id) && (C\is_empty($session->items()) || !$session->changed())) {
            return $response;
        }

        // Regenerate the session if:
        // - we have no session identifier
        // - the session is marked as regenerated
        // - the session has changed (data is different)
        if (Str\is_empty($id) || $session->regenerated() || $session->changed()) {
            $id = $this->regenerateSession();
        }

        $_SESSION = $session->items();
        session_commit();

        return $this->withCacheHeaders(
            $response->withCookie(
                $this->options['cookie']['name'] ?? 'hh-session', 
                $this->createCookie($id, $session->age())
            )
        );        
    }

    private function startSession(string $id, dict<string, mixed> $options = dict[]): void
    {
        $iniConfig = dict[
            'session.use_cookies' => ini_set('session.use_cookies', false),
            'session.use_only_cookies' => ini_set('session.use_only_cookies', true),
            'session.cache_limiter' => ini_set('session.cache_limiter', '')
        ];
        foreach ($options as $key => $value) {
            $key = Str\starts_with($key, 'session.') ? $key : 'session.' . $key;
            $iniConfig[$key] = ini_set($key, $value);
        }
        session_id($id);
        session_start();
        foreach($iniConfig as $key => $value) {
            ini_set($key, $value);
        }
    }

    private function regenerateSession(): string
    {
        session_commit();
        $id = $this->generateSessionId();
        $this->startSession($id, dict[
            'use_strict_mode' => false
        ]);
        return $id;    
    }
}