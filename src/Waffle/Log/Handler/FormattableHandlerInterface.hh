<?hh // strict

namespace Waffle\Log\Handler;

use type Waffle\Log\Formatter\FormatterInterface;

interface FormattableHandlerInterface extends HandlerInterface
{
    /**
     * Sets the formatter.
     */
    public function setFormatter(FormatterInterface $formatter): this;

    /**
     * Gets the formatter.
     */
    public function getFormatter(): FormatterInterface;
}
