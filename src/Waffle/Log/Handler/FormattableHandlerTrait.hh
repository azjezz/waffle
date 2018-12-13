<?hh // strict

namespace Waffle\Log\Handler;

use type Waffle\Log\Formatter\FormatterInterface;
use type Waffle\Log\Formatter\LineFormatter;

trait FormattableHandlerTrait
{
    require implements FormattableHandlerInterface;

    protected ?FormatterInterface $formatter = null;

    public function setFormatter(FormatterInterface $formatter): this
    {
        $this->formatter = $formatter;

        return $this;
    }

    public function getFormatter(): FormatterInterface
    {
        if (!$this->formatter) {
            $this->formatter = $this->getDefaultFormatter();
        }

        return $this->formatter;
    }

    /**
     * Gets the default formatter.
     *
     * Overwrite this if the LineFormatter is not a good default for your handler.
     */
    protected function getDefaultFormatter(): FormatterInterface
    {
        return new LineFormatter();
    }
}
