<?hh // strict

namespace Waffle\Contract\Event;

type EventListener = (function(EventInterface): void);
