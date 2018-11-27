<?php
/**
 * @see       https://github.com/zendframework/zend-diactoros for the canonical source repository
 * @copyright Copyright (c) 2015-2018 Zend Technologies USA Inc. (http://www.zend.com)
 * @license   https://github.com/zendframework/zend-diactoros/blob/master/LICENSE.md New BSD License
 */

declare(strict_types=1);

namespace Waffle\Http\Message\Response;

use Waffle\Contract\Http\Message\StreamInterface;
use Waffle\Http\Message\Exception;
use Waffle\Http\Message\Response;
use Waffle\Http\Message\Stream;

use function get_class;
use function gettype;
use function is_object;
use function is_string;
use function sprintf;
use function array_reduce;
use function strtolower;

/**
 * XML response.
 *
 * Allows creating a response by passing an XML string to the constructor; by default,
 * sets a status code of 200 and sets the Content-Type header to application/xml.
 */
class XmlResponse extends Response
{
    /**
     * Create an XML response.
     *
     * Produces an XML response with a Content-Type of application/xml and a default
     * status of 200.
     */
    public function __construct(string $xml, int $status = 200, Map<string, Set<string>> $headers = Map {})
    {
        parent::__construct(
            $status,
            Functional\InjectContentTypeInHeaders('application/xml; charset=utf8', $headers),
            Functional\CreateStreamFromString($xml),
        );
    }
}
