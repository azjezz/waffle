<?hh // strict

namespace Waffle\Http\Message;

use namespace HH\Lib\Str;
use type Waffle\Contract\Http\Message\StreamInterface;
use type Waffle\Contract\Http\Message\MessageInterface;
use function preg_match;

/**
 * Trait implementing functionality common to requests and responses.
 */
trait MessageTrait
{
    require implements MessageInterface;

    /** @var Map<string, Set<string>> Map of all registered headers, as original name => Set of values */
    protected Map<string, Set<string>> $headers = Map {};

    /** @var Map<string, string> Map of lowercase header name => original name at registration */
    protected Map<string, string> $headerNames = Map {};

    protected string $protocol = '1.1';

    protected ?StreamInterface $stream;

    public function __clone(): void
    {
        $this->headers = clone $this->headers;
        $this->stream = null === $this->stream ? null : (clone $this->stream);
        $this->headerNames = clone $this->headerNames;
    }

    public function getProtocolVersion(): string
    {
        return $this->protocol;
    }

    public function withProtocolVersion(string $version): this
    {
        if ($this->protocol === $version) {
            return $this;
        }

        $new = clone $this;
        $new->protocol = $version;

        return $new;
    }

    public function getHeaders():  Map<string, Set<string>>
    {
        return $this->headers;
    }

    public function hasHeader(string $header): bool
    {
        return $this->headerNames->contains(Str\lowercase($header));
    }

    public function getHeader(string $header): Set<string>
    {
        $header = Str\lowercase($header);

        $header = $this->headerNames->get($header);

        if (null === $header) {
            return Set {};
        }

        return $this->headers->get($header) ?? Set {};
    }

    public function getHeaderLine(string $header): string
    {
        return Str\join($this->getHeader($header), ', ');
    }

    public function withHeader(string $header,Set<string> $value): this
    {
        $value = $this->validateAndTrimHeader($header, $value);

        $normalized = Str\lowercase($header);

        $new = clone $this;

        if ($new->headerNames->contains($normalized)) {
            $new->headers->remove(
                /* HH_IGNORE_ERROR[4110] */
                /* we already know that 'headerNames' contains this key */
                $new->headerNames->get($normalized)
            );
        }

        $new->headerNames->set($normalized, $header);
        $new->headers->set($header, $value);

        return $new;
    }

    public function withAddedHeader(string $header, Set<string> $value): this
    {
        if ('' === $header) {
            throw new Exception\InvalidArgumentException('Header name must be an RFC 7230 compatible string.');
        }

        $new = clone $this;
        $new->setHeaders(Map {
            $header => $value
        });

        return $new;
    }

    public function withoutHeader(string $header): this
    {
        $normalized = Str\lowercase($header);

        if (!$this->headerNames->contains($normalized)) {
            return $this;
        }

        $header = $this->headerNames->at($normalized);

        $new = clone $this;

        $new->headerNames->remove($normalized);
        $new->headers->remove($header);

        return $new;
    }

    public function getBody(): StreamInterface
    {
        if (!$this->stream) {
            $this->stream = Functional\create_stream_from_string('');
        }

        return $this->stream;
    }

    public function withBody(StreamInterface $body): this
    {
        if ($body === $this->stream) {
            return $this;
        }

        $new = clone $this;
        $new->stream = $body;

        return $new;
    }

    private function setHeaders(Map<string, Set<string>> $headers): void
    {
        foreach ($headers as $header => $value) {
            $value = $this->validateAndTrimHeader($header, $value);

            $normalized = Str\lowercase($header);

            if ($this->headerNames->contains($normalized)) {

                $header = $this->headerNames->get($normalized);

                if (null !== $header) {
                    $this->headers->set(
                        $header,
                        new Set<string>(
                            $this->headers->get($header)
                                ?->concat($value)
                                ?->toArray()
                        )
                    );
                }
            } else {
                $this->headerNames->set($normalized, $header);
                $this->headers->set($header, $value);
            }
        }
    }

    /**
     * Make sure the header complies with RFC 7230.
     *
     * Header names must be a non-empty string consisting of token characters.
     *
     * Header values must be strings consisting of visible characters with all optional
     * leading and trailing whitespace stripped. This method will always strip such
     * optional whitespace. Note that the method does not allow folding whitespace within
     * the values as this was deprecated for almost all instances by the RFC.
     *
     * header-field = field-name ":" OWS field-value OWS
     * field-name   = 1*( "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^"
     *              / "_" / "`" / "|" / "~" / %x30-39 / ( %x41-5A / %x61-7A ) )
     * OWS          = *( SP / HTAB )
     * field-value  = *( ( %x21-7E / %x80-FF ) [ 1*( SP / HTAB ) ( %x21-7E / %x80-FF ) ] )
     *
     * @see https://tools.ietf.org/html/rfc7230#section-3.2.4
     */
    private function validateAndTrimHeader(string $header, Set<string> $values): Set<string>
    {
        if (1 !== preg_match("@^[!#$%&'*+.^_`|~0-9A-Za-z-]+$@", $header)) {
            throw new Exception\InvalidArgumentException(
                'Header name must be an RFC 7230 compatible string.'
            );
        }

        if (0 === $values->count()) {
            throw new Exception\InvalidArgumentException(
                'Header values must be a Set of strings, empty Set given.'
            );
        }

        $retval = Set {};

        foreach ($values as $value) {

            if (1 !== preg_match("@^[ \t\x21-\x7E\x80-\xFF]*$@", $value)) {
                throw new Exception\InvalidArgumentException(
                    'Header values must be RFC 7230 compatible strings.'
                );
            }

            $retval->add(
                Str\trim($value, " \t")
            );
        }

        return $retval;
    }
}
