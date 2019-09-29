#ifndef EVENT_TRAITS_H
#define EVENT_TRAITS_H

#include <functional>

namespace Traits
{

template<typename TEvent>
struct EventTraits
{
    using handler_t = std::function<void(const TEvent&)>;
};

template<typename TEvent>
using event_handler_t = typename EventTraits<TEvent>::handler_t;
}

#endif
