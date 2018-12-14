<?hh // strict

namespace Waffle\Http\Message\__Private;

use namespace HH\Lib\Str;
use namespace Waffle\Http\Message\Exception;
use type Waffle\Http\Message\Stream;
use type Waffle\Http\Message\UploadedFile;
use type Waffle\Http\Message\UploadsFolder;
use type Waffle\Contract\Http\Message\UploadedFileError;
use type Waffle\Contract\Http\Message\UploadedFileInterface;
use type Waffle\Contract\Http\Message\UploadsFolderInterface;
use function is_readable;
use function is_array;
use function fopen;

type file_spec = shape(
    'tmp_name' => string,
    'size' => ?int,
    'error' => int,
    ?'name' => string,
    ?'type' => string,
    ...
);

class UploadsFolderMarshaler
{
    public function marshal(KeyedContainer<string, mixed> $files): UploadsFolderInterface
    {
        $uploads = new UploadsFolder();

        foreach ($files as $key => $value) {
            if ($value is UploadedFileInterface) {
                $uploads = $uploads->withFile($key, $value);
                continue;
            }

            if ($value is UploadsFolderInterface) {
                $uploads = $uploads->withFolder($key, $value);
                continue;
            }

            if (is_array($value)) {
                $value = new Map<string, mixed>($value);
                if ($value->contains('tmp_name')) {
                    if (is_array($value->get('tmp_name'))) {
                        if (! $value->contains('size') || ! is_array($value->at('size'))
                            || ! $value->contains('error') || ! is_array($value->at('error'))
                        ) {
                            throw new Exception\InvalidArgumentException(Str\format(
                                '$files provided to %s MUST contain each of the keys "tmp_name",'
                                . ' "size", and "error", with each represented as an array;'
                                . ' one or more were missing or non-array values.',
                                __FUNCTION__
                            ));
                        }

                        $uploads = $uploads->withFolder(
                            $key,
                            $this->recursiveNormalize(
                                /* HH_IGNORE_ERROR[4110] */
                                $value->at('tmp_name'),
                                /* HH_IGNORE_ERROR[4110] */
                                $value->at('size'),
                                /* HH_IGNORE_ERROR[4110] */
                                $value->at('error'),
                                /* HH_IGNORE_ERROR[4110] */
                                $value->get('name'),
                                /* HH_IGNORE_ERROR[4110] */
                                $value->get('type')
                            )
                        );
                    } else {
                        $value = $value->toArray();
                        $uploads = $uploads->withFile($key, $this->createUploadedFile($value));
                    }
                    continue;
                }

                $value = $value->toArray();
                $uploads = $uploads->withFolder(
                    $key,
                    $this->marshal($value)
                );
                continue;
            }

            throw new Exception\InvalidArgumentException('Invalid value in files specification');
        }

        return $uploads;
    }

    private function recursiveNormalize(
        array<string, mixed> $tmpNameTree,
        array<string, mixed> $sizeTree,
        array<string, mixed> $errorTree,
        array<string, mixed> $nameTree = [],
        array<string, mixed> $typeTree = []
    ): UploadsFolder  {
        $folder = new UploadsFolder();

        foreach ($tmpNameTree as $key => $value) {
            if (is_array($value)) {
                // Traverse
                $folder = $folder->withFolder(
                    $key,
                    $this->recursiveNormalize(
                        /* HH_IGNORE_ERROR[4110] */
                        $tmpNameTree[$key],
                        /* HH_IGNORE_ERROR[4110] */
                        $sizeTree[$key],
                        /* HH_IGNORE_ERROR[4110] */
                        $errorTree[$key],
                        /* HH_IGNORE_ERROR[4110] */
                        $nameTree[$key] ?? null,
                        /* HH_IGNORE_ERROR[4110] */
                        $typeTree[$key] ?? null
                    )
                );
                continue;
            }

            $folder = $folder->withFile(
                $key,
                $this->createUploadedFile([
                    'tmp_name' => $tmpNameTree[$key],
                    'size' => $sizeTree[$key],
                    'error' => $errorTree[$key],
                    'name' => $nameTree[$key],
                    'type' => $typeTree[$key]
                ])
            );

        }

        return $folder;
    }

    private function createUploadedFile(array<arraykey, mixed> $fileSpec): UploadedFileInterface
    {
        if (!$fileSpec is file_spec) {
            throw new Exception\InvalidArgumentException(Str\format(
                '$fileSpec provided to %s MUST contain each of the keys "tmp_name", '
                . ' "size", and "error"; one or more were missing',
                __FUNCTION__
            ));
        }

        $tmpName = $fileSpec['tmp_name'];

        if (!is_readable($tmpName)) {
            throw new Exception\InvalidArgumentException(Str\format(
                'uploaded file "%s" is not readable',
                $fileSpec['tmp_name']
            ));
        }

        $stream = new Stream(fopen($tmpName, 'r'));

        $error = $fileSpec['error'];

        if ($error > 8 || $error < 0) {
            throw new Exception\InvalidArgumentException('Invalid error status for UploadedFile; must be an UPLOAD_ERR_* constant');
        } else {
            $error = UploadedFileError::coerce($error);
        }

        return new UploadedFile(
            $stream,
            $fileSpec['size'],
            /* HH_IGNORE_ERROR[4110] */
            $error,
            $fileSpec['name'] ?? null,
            $fileSpec['type'] ?? null,
        );
    }
}
