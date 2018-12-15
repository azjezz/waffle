<?hh // strict

namespace Waffle\Log;

use type Waffle\Contract\Log\LogLevel;
use type DateTimeInterface;

type record = shape(
    'level' => LogLevel,
    'message' => string,
    'context' => dict<string, mixed>,
    'time' => DateTimeInterface,
    'extra' => dict<string, mixed>,
    ?'formatted' => string,
    ...
);
