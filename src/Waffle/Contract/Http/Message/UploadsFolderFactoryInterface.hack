<?hh

namespace Waffle\Contract\Http\Message;

interface UploadsFolderFactoryInterface
{
    /*
     * Create am empty uploads folder instance.
     */
    public function createUploadsFolder(): UploadsFolderInterface;
}
