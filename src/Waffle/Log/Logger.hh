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
}
