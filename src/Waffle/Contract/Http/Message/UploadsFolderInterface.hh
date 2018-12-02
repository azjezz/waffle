<?hh

namespace Waffle\Contract\Http\Message;

/**
 * This class represents upload metadata in a normalized tree, with each leaf
 * an instance of Waffle\Contract\Http\Message\UploadedFileInterface.
 */
interface UploadsFolderInterface
{
    /**
     * Retrieve a file from the uploads folder with the given key.
     * If the file has not been previously set, returns
     * null.
     *
     * @return ?UploadedFileInterface an UploadedFileInterface instance or null if not found.
     */
    public function getFile(arraykey $key): ?UploadedFileInterface;

    /**
     * Retrieve a sub-folder from the current uploads folder with the given name.
     * If the folder has not been previously set, returns
     * null.
     *
     * @return ?UploadsFolderInterface the sub-folder or null if it doesn't exist.
     */
    public function getFolder(string $name): ?UploadsFolderInterface;

    /**
     * Create a new instance with the specified UploadedFileInterface.
     *
     * This method MUST be implemented in such a way as to retain the
     * immutability of the folder, and MUST return an instance that has the
     * new file added to the files container.
     *
     * @see getFile()
     */
    public function withFile(arraykey $key, UploadedFileInterface $file): this;

    /**
     * Create a new instance with the specified UploadsFolderInterface as a sub-folder.
     *
     * This method MUST be implemented in such a way as to retain the
     * immutability of the folder, and MUST return an instance that has the
     * new sub-folder added to the folders container.
     *
     * @see getFolder()
     */
    public function withFolder(string $name, UploadsFolderInterface $folder): this;

    /**
     * Return an instance that removes the specified uploaded file.
     *
     * This method allows removing a single UploadedFileInterface instance as 
     * described in getFile().
     *
     * This method MUST be implemented in such a way as to retain the
     * immutability of the folder, and MUST return an instance that removes
     * the file.
     *
     * @param arraykey $key The file key.
     */
    public function withoutFile(arraykey $key): this;

    /**
     * Return an instance that removes the specified uploads sub-folder.
     *
     * This method allows removing a single UploadsFolderInterface instance as 
     * described in getFolder().
     *
     * This method MUST be implemented in such a way as to retain the
     * immutability of the folder, and MUST return an instance that removes
     * the sub-folder.
     *
     * @param string $name The folder name.
     */
    public function withoutFolder(string $name): this;
}
