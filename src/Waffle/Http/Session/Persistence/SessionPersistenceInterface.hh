<?hh // strict

namespace Waffle\Http\Session\Persistence;

use type Waffle\Contract\Http\Message\ServerRequestInterface;
use type Waffle\Contract\Http\Message\ResponseInterface;
use type Waffle\Contract\Http\Session\SessionInterface;

interface SessionPersistenceInterface
{
    /**
     * Generate a session data instance based on the request.
     */
    public function initialize(ServerRequestInterface $request): SessionInterface;

    /**
     * Persist the session data instance
     *
     * Persists the session data, returning a response instance with any
     * artifacts required to return to the client.
     */
    public function persist(SessionInterface $session, ResponseInterface $response): ResponseInterface;
}
