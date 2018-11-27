<?hh // strict

namespace Waffle\Http\Message\Response;

use namespace HH\Lib\Str;
use Waffle\Http\Message\Exception;
use Waffle\Http\Message\Response;
use Waffle\Http\Message\Stream;
use Waffle\Http\Message\Functional;

use function is_object;
use function json_encode;
use function json_last_error;
use function json_last_error_msg;
use function array_reduce;

use const JSON_ERROR_NONE;

/**
 * JSON response.
 *
 * Allows creating a response by passing data to the constructor; by default,
 * serializes the data to JSON, sets a status code of 200 and sets the
 * Content-Type header to application/json.
 */
class JsonResponse extends Response
{
    /**
     * Default flags for json_encode; value of:
     *
     * <code>
     * JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_AMP | JSON_HEX_QUOT | JSON_UNESCAPED_SLASHES
     * </code>
     *
     * @const int
     */
    const DEFAULT_JSON_FLAGS = 79;

    private mixed $payload;

    private int $encodingOptions;

    /**
     * Create a JSON response with the given data.
     *
     * Default JSON encoding is performed with the following options, which
     * produces RFC4627-compliant JSON, capable of embedding into HTML.
     *
     * - JSON_HEX_TAG
     * - JSON_HEX_APOS
     * - JSON_HEX_AMP
     * - JSON_HEX_QUOT
     * - JSON_UNESCAPED_SLASHES
     *
     * @param mixed $data Data to convert to JSON.
     * @param int $status Integer status code for the response; 200 by default.
     * @param Map<string, Set<string>> $headers Map of headers to use at initialization.
     * @param int $encodingOptions JSON encoding options to use.
     * @throws Exception\InvalidArgumentException if unable to encode the $data to JSON.
     */
    public function __construct(
        mixed $data,
        int $status = 200,
        Map<string, Set<string>> $headers = Map {},
        int $encodingOptions = self::DEFAULT_JSON_FLAGS
    ) {
        $this->setPayload($data);
        $this->encodingOptions = $encodingOptions;

        $json = $this->jsonEncode($data, $this->encodingOptions);
        $body = Functional\CreateStreamFromString($json);

        $headers = Functional\InjectContentTypeInHeaders('application/json', $headers);

        parent::__construct($status, $headers, $body);
    }

    /**
     * @return mixed
     */
    public function getPayload(): mixed
    {
        return $this->payload;
    }

    public function withPayload(mixed $data): JsonResponse
    {
        $new = clone $this;
        $new->setPayload($data);
        return $this->updateBodyFor($new);
    }

    public function getEncodingOptions(): int
    {
        return $this->encodingOptions;
    }

    public function withEncodingOptions(int $encodingOptions): JsonResponse
    {
        $new = clone $this;
        $new->encodingOptions = $encodingOptions;
        return $this->updateBodyFor($new);
    }

    /**
     * Encode the provided data to JSON.
     *
     * @param mixed $data
     * @throws Exception\InvalidArgumentException if unable to encode the $data to JSON.
     */
    private function jsonEncode(mixed $data, int $encodingOptions): string
    {
        if ($data is resource) {
            throw new Exception\InvalidArgumentException('Cannot JSON encode resources');
        }

        // Clear json_last_error()
        json_encode(null);

        $json = json_encode($data, $encodingOptions);

        if (JSON_ERROR_NONE !== json_last_error()) {
            throw new Exception\InvalidArgumentException(Str\format(
                'Unable to encode data to JSON in %s: %s',
                __CLASS__,
                json_last_error_msg()
            ));
        }

        return $json;
    }

    /**
     * @param mixed $data
     */
    private function setPayload(mixed $data): void
    {
        if (is_object($data)) {
            /* HH_IGNORE_ERROR[4110] $data is an object*/
            $data = clone $data;
        }

        $this->payload = $data;
    }

    /**
     * Update the response body for the given instance.
     *
     * @param self $toUpdate Instance to update.
     * @return JsonResponse Returns a new instance with an updated body.
     */
    private function updateBodyFor(JsonResponse $toUpdate): JsonResponse
    {
        $json = $this->jsonEncode($toUpdate->payload, $toUpdate->encodingOptions);
        $body = Functional\CreateStreamFromString($json);
        return $toUpdate->withBody($body);
    }

}
