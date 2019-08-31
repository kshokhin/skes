#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include "EventDispatcher.h"
#include "LocalEventSubscriptionPolicy.h"

using event_dispatcher_t = EventDispatcher<LocalEventSubscriptionPolicy>;
using player_id_t = uint8_t;

#endif
