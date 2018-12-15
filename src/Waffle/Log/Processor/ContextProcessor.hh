<?hh

namespace Waffle\Log\Processor;

use namespace HH\Lib\Str;
use type Waffle\Log\record;
use type Waffle\Log\__Private\VariableNormalizer;

class ContextProcessor implements ProcessorInterface
{
    const string SIMPLE_DATE = "Y-m-d\TH:i:s.uP";

    private VariableNormalizer $normalizer;

    public function __construct(
        ?string $dateFormat = null,
    ) {
        $this->normalizer = new VariableNormalizer($dateFormat ?? static::SIMPLE_DATE);
    }

    public function process(record $record): record
    {
        if (!Str\contains($record['message'], '}')) {
            return $record;
        }

        foreach ($record['context'] as $key => $value) {
            $placeholder = '{' . $key . '}';

            if (!Str\contains($record['message'], $placeholder)) {
                continue;
            }

            $record['message'] = Str\replace(
                $record['message'],
                $placeholder,
                $this->normalizer->normalize($value)
            );
        }

        return $record;
    }
}
