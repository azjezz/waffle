<?hh // strict

namespace Waffle\Contract\Http\Client;

use type Exception;

/**
 * Every HTTP client related exception MUST implement this interface.
 */
interface ClientExceptionInterface
{
    require extends Exception;
}
