<?hh // strict

namespace Waffle\Log\Processor;

use type Waffle\Log\record;

interface ProcessorInterface
{
    public function process(record $record): record;
}
