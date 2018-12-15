<?hh // strict

namespace Waffle\Log;

use type Waffle\Contract\Log\LoggerInterface;
use type Waffle\Contract\Log\AbstractLogger;
use type Waffle\Contract\Log\LogLevel;
use type DateTime;

class Logger extends AbstractLogger implements LoggerInterface
{
    public function __construct(
        public vec<Handler\HandlerInterface> $handlers,
        public vec<Processor\ProcessorInterface> $processors
    ) {}

    <<__Override>>
    public function log(LogLevel $level, string $message, dict<string, mixed> $context = dict[]): void
    {
        $record = shape(
            'level' => $level,
            'message' => $message,
            'context' => $context,
            'time' => new DateTime('now'),
            'extra' => dict[]
        );

        foreach ($this->processors as $processor) {
            $record = $processor->process($record);
        }

        foreach ($this->handlers as $handler) {
            if (!$handler->isHandling($record)) {
                continue;
            }

            if ($handler->handle($record)) {
                break;
            }
        }
    }

    /**
     * Ends a log cycle and frees all resources used by handlers.
     *
     * Closing a Handler means flushing all buffers and freeing any open resources/handles.
     * Handlers that have been closed should be able to accept log records again and re-open
     * themselves on demand, but this may not always be possible depending on implementation.
     *
     * This is useful at the end of a request and will be called automatically on every handler
     * when they get destructed.
     */
    public function close(): void
    {
        foreach ($this->handlers as $handler) {
            $handler->close();
        }
    }
}
