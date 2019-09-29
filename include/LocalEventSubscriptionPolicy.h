#ifndef LOCAL_EVENT_SUBSCRIBERS_POLICY_H
#define LOCAL_EVENT_SUBSCRIBERS_POLICY_H

#include <unordered_map>
#include <functional>
#include <vector>

#include "EventSubscribers.h"

class LocalEventSubscriptionPolicy
{
public:
    template<typename TEvent>
    void postEvent(size_t event_id, EventProcessor* publisher, const TEvent& event);

    template<typename TEvent>
    void subscribeEvent(size_t event_id, EventProcessor& subscriber,
        const typename Traits::event_handler_t<TEvent>& handler);

    template<typename TEvent>
    void subscribeEvent(size_t event_id, EventProcessor& subscriber,
        const typename Traits::event_handler_t<TEvent>& handler,
        const typename Traits::event_filter_t<TEvent>& filter);

    template<typename TEvent>
    void unsubscribeEvent(size_t event_id, EventProcessor& subscriber);
private:
    std::unordered_map<size_t, EventSubscribers> m_subscribers;
};

template<typename TEvent>
void LocalEventSubscriptionPolicy::postEvent(
    size_t event_id, EventProcessor* publisher, const TEvent& event)
{
    auto subscribers_it = m_subscribers.find(event_id);
    if (subscribers_it == std::end(m_subscribers)) return;
    subscribers_it->second.template post<TEvent>(publisher, event);
}

template<typename TEvent>
void LocalEventSubscriptionPolicy::subscribeEvent(
    size_t event_id, EventProcessor& subscriber,
    const typename Traits::event_handler_t<TEvent>& handler)
{ 
    m_subscribers[event_id].template subscribe<TEvent>(
            subscriber, handler);
}

template<typename TEvent>
void LocalEventSubscriptionPolicy::subscribeEvent(
    size_t event_id, EventProcessor& subscriber,
    const typename Traits::event_handler_t<TEvent>& handler,
    const typename Traits::event_filter_t<TEvent>& filter)
{ 
    m_subscribers[event_id].template subscribe<TEvent>(
            subscriber, handler, filter);
}

template<typename TEvent>
void LocalEventSubscriptionPolicy::unsubscribeEvent(
    size_t event_id, EventProcessor& subscriber)
{ 
    auto subscribers_it = m_subscribers.find(event_id);
    if (subscribers_it == std::end(m_subscribers)) return;

    subscribers_it->second.unsubscribe(subscriber);
}

#endif
