<?hh // strict

namespace Waffle\Http\Session;

use type Waffle\Contract\Http\Message\CookieSameSite;

type SessionOptions = shape(
    ?'cookie' => shape(
        /**
         * Specifies the session cookie name. it should
         * only contain alphanumeric characters.
         * Defaults to "hh-session".
         */
        ?'name' => string,

        /**
         * Specifies the lifetime of the cookie in seconds
         * which is sent to the browser.
         * The value 0 means "until the browser is closed.".
         * Defauls to 0.
         */
        ?'lifetime' => int,

        /**
         * Specifies path to set in the session cookie.
         * Default to "/".
         */
        ?'path' => string,

        /**
         * Specifies the domain to set in the session cookie.
         * Default is none at all meaning the host name of
         * the server which generated the cookie according to
         * cookies specification.
         */
        ?'domain' => string,

        /**
         * Specifies whether the cookie should only be sent over
         * secure connections.
         * Defaults to false.
         */
        ?'secure' => bool,

        /**
         * Marks the cookie as accessible only through the HTTP
         * Protocol. this means that cookie won't be accessible by
         * scripting langauges, such as JavaScript. this setting can
         * effectively help to reduce identity theft through XSS attacks.
         * Note that this is not supported by all browsers.
         * Defaults to false.
         */
        ?'http_only' => bool,

        /**
         * Allows servers to assert that a cookie ought not
         * to be sent along with cross-site requests.
         * This assertion allows user agents to mitigate the risk
         * of cross-origin information leakage, and provides some
         * protection against CSRF attacks.
         * Note that this is not supported by all browsers.
         */
        ?'same_site' => CookieSameSite,
        ...
    ),

    /**
     * pecifies the cache control method used for session pages.
     * Defaults to null (won't send any cache headers).
     */
    ?'cache_limiter' => ?CacheLimiter,

    /**
     * Specifies time-to-live for cached session pages in minutes,
     * This has no effect for CacheLimiter::NOCACHE.
     * Defaults to 180
     */
    ?'cache_expire' => int,

    /**
     * Specifies the session persistence implementation to use.
     * Defaults to the native session persistence.
     *
     * Built-in persistence : 'cache', 'native'.
     */
    ?'persistence' => string,

    ...
);
