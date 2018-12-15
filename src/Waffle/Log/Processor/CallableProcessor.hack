<?hh // strict

namespace Waffle\Log\Processor;

use type Waffle\Log\record;

class CallableProcessor implements ProcessorInterface
{
    public function __construct(
        protected (function(record): record) $callable
    ) {}

    public function process(record $record): record
    {
        $fun = $this->callable;

        return $fun($record);
    }
}
