#ifndef EVENT_TRAITS_H
#define EVENT_TRAITS_H

#include <functional>

class EventProcessor;

namespace Traits
{

template<typename TEvent>
struct EventTraits
{
    using handler_t = std::function<void(const TEvent&)>;
    using filter_t = std::function<bool(const TEvent&)>;
};

template<typename TEvent>
using event_handler_t = typename EventTraits<TEvent>::handler_t;

template<typename TEvent>
using event_filter_t = typename EventTraits<TEvent>::filter_t;
}

#endif
