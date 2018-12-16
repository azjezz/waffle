<?hh // strict

namespace Waffle\Contract\Event;

type EventListener<TEvent super EventInterface> = (function(TEvent): mixed);
