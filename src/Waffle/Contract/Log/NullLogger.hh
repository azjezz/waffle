<?hh // strict

namespace Waffle\Contract\Log;

use Map;

/**
 * This Logger can be used to avoid conditional log calls.
 *
 * Logging should always be optional, and if no logger is provided to your
 * library creating a NullLogger instance to have something to throw logs at
 * is a good way to avoid littering your code with `if ($this->logger) { }`
 * blocks.
 */
class NullLogger extends AbstractLogger
{
    /**
     * Logs with an arbitrary level.
     *
     * @param LogLevel  $level
     * @param string    $message
     * @param Map       $context
     *
     * @return void
     */
    public function log(LogLevel $level, string $message, array<arraykey, mixed> $context = []): void
    {
        // noop
    }
}
