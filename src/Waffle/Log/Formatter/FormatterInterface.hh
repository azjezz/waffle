<?hh // strict

namespace Waffle\Log\Formatter;

use type Waffle\Log\record;

interface FormatterInterface
{
    public function format(record $record): record;
}
