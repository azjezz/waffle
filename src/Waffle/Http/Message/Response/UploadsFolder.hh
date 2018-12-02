<?hh

namespace Waffle\Http\Message;

use type Waffle\Contract\Http\Message\UploadsFolderInterface;
use type Waffle\Contract\Http\Message\UploadedFileInterface;

class UploadsFolder implements UploadsFolderInterface
{
    protected Map<arraykey, UploadedFileInterface> $files = Map {};

    protected Map<string, UploadsFolderInterface> $folders = Map {};

    public function __clone(): void
    {
        $this->files = clone $this->files;
        $this->folders = clone $this->folders;
    }

    public function getFile(arraykey $key): ?UploadedFileInterface
    {
        return $this->files->get($key);
    }

    public function getFolder(string $name): ?UploadsFolderInterface
    {
        return $this->folders->get($name);
    }

    public function withFile(arraykey $key, UploadedFileInterface $file): this
    {
        $new = clone $this;
        $new->files->set($key, $file);

        return $new;
    }

    public function withFolder(string $name, UploadsFolderInterface $folder): this
    {
        $new = clone $this;
        $new->folders->set($name, $folder);

        return $new;
    }

    public function withoutFile(arraykey $key): this
    {
        $new = clone $this;
        $new->files->remove($key);

        return $new;
    }

    public function withoutFolder(string $name): this
    {
        $new = clone $this;
        $new->folders->remove($name);

        return $new;
    }
}