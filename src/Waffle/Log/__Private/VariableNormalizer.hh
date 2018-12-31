<?hh // strict

namespace Waffle\Log\__Private;

use namespace HH\Lib\Str;
use type Waffle\Lib\Json;
use type DateTimeInterface;
use type Throwable;
use function is_object;
use function get_resource_type;
use function get_class;
use function gettype;

final class VariableNormalizer
{
    const string SIMPLE_DATE = "Y-m-d\TH:i:s.uP";

    private string $dateFormat;

    public function __construct(?string $dateFormat = null)
    {
        $this->dateFormat = $dateFormat ?? VariableNormalizer::SIMPLE_DATE;
    }

    public function normalize(mixed $value): string
    {
        if ($value is bool) {
            $value = ($value ? 'true' : 'false');
        } elseif ($value is string) {
            $value = '"'.$value.'"';
        } elseif ($value is num) {
            $value = $value is int ? $value : Str\format_number($value, 1);
        } elseif ($value is resource) {
            $value = 'resource['.get_resource_type($value).']';
        } elseif (null === $value) {
            $value = 'null';
        } elseif (is_object($value) && !$value is Container<_>) {
            if ($value is Throwable) {
                $value = get_class($value).'['.
                    'message=' . $this->normalize($value->getMessage()) .
                    ', code=' . $this->normalize($value->getCode()) .
                    ', file=' . $this->normalize($value->getFile()) .
                    ', line=' . $this->normalize($value->getLine()) .
                    ', trace= ' . $this->normalize($value->getTrace()) .
                    ', previous=' . $this->normalize($value->getPrevious()) .
                    ']';
            } elseif ($value is DateTimeInterface) {
                $value = get_class($value) . '['. $value->format($this->dateFormat) .']';
            } else {
                $value = 'object[' . get_class($value) . ']';
            }
        } elseif ($value is Container<_>) {
            $value = Json::encode($value, false);
        } else {
            $value = '!' . gettype($value).Json::encode($value, false);
        }

        return (string) $value;
    }
}
