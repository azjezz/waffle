<?hh // strict

namespace Waffle\Contract\Log;

/**
 * Basic Implementation of LoggerAwareInterface.
 */
trait LoggerAwareTrait
{
    require implements LoggerAwareInterface;

    /**
     * The logger instance.
     */
    protected ?LoggerInterface $logger;

    /**
     * Sets a logger.
     */
    public function setLogger(LoggerInterface $logger): void
    {
        $this->logger = $logger;
    }
}