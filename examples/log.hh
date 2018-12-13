<?hh  // strict

namespace Examples\Log;

use namespace Waffle\Log\Processor;
use namespace Waffle\Log\Handler;
use namespace Waffle\Log\Formatter;
use type Waffle\Log\Logger;
use type Waffle\Contract\Log\LogLevel;
use type RuntimeException;
use type Exception;

require '../vendor/hh_autoload.php';

<<__Entrypoint>>
function main(): void
{
    $handler = new Handler\StreamHandler(__DIR__.'/log.txt', LogLevel::INFO);
    $handler->setFormatter(
        new Formatter\LineFormatter("[%time%][%level%] => %message% \n", 'Y-m-d H:i:s')
    );

    $processor = new Processor\ContextProcessor();

    $logger = new Logger(vec[ $handler ], vec[ $processor ]);

    // this won't be logged as the minimum level has been set to "info"
    $logger->debug('Detailed debug information.');
    
    $logger->info('Interesting events.');

    $logger->notice('Normal but significant events.');

    $logger->warning('Exceptional occurrences that are not errors.');

    $logger->error('Runtime errors that do not require immediate action but should typically be logged and monitored.');

    $logger->critical("Critical conditions.Application component unavailable, unexpected exception. {error}", dict[
        'error' => new Exception('message !', 21, new RuntimeException('Ops'))
    ]);

    $logger->alert('Action must be taken immediately. Entire website down, database unavailable, etc.');

    $logger->emergency('System is unusable.');
}

