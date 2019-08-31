#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <optional>

#include "types.h"
#include "EventProcessor.h"
#include "EventTraits.h"
#include "LocalEventSubscribersPolicy.h"

class EventDispatcher
{
public:
    template<typename TEvent>
    void postEvent(EventProcessor* publisher, const TEvent& event);

    template<typename TEvent>
    void subscribeEvent(EventProcessor& subscriber,
        const typename Traits::event_handler_t<TEvent>& handler);

    template<typename TEvent>
    void unsubscribeEvent(EventProcessor& subscriber);
private:
    template<typename TEvent>
    size_t eventID()
    {
        static const size_t id = m_next_event_id++;

        return id;
    }

    void checkSubscriberID(EventProcessor& subscriber)
    {
        if (subscriber.hasID()) return;

        subscriber.setID(newSubscriberID());
    }

    static size_t newSubscriberID() {
        static std::atomic<size_t> next_subscriber_id = 0;

        return next_subscriber_id++;
    }
private:
    size_t m_next_event_id = 0;
    std::unordered_map<size_t, EventSubscribers> m_subscribers;
};

template<typename TEvent>
void EventDispatcher::postEvent(EventProcessor* publisher, const TEvent& event)
{
    if (publisher != nullptr)
    {
        checkSubscriberID(*publisher);
    }

    std::cout << "postEvent() | " << "publisher ";
    std::cout << (publisher == nullptr ? "null" : std::to_string(publisher->id()));
    std::cout << " post event " << eventID<TEvent>() << "\n";

    auto subscribers_it = m_subscribers.find(eventID<TEvent>());
    if (subscribers_it == std::end(m_subscribers)) return;
    subscribers_it->second.template post<TEvent>(publisher, event);
}

template<typename TEvent>
void EventDispatcher::subscribeEvent(EventProcessor& subscriber,
    const typename Traits::event_handler_t<TEvent>& handler)
{
    checkSubscriberID(subscriber);
    std::cout << "subscribeEvent() | subscriber " << subscriber.id();
    std::cout << " subscribed to event " << eventID<TEvent>() << "\n";
    m_subscribers[eventID<TEvent>()].template subscribe<TEvent>(subscriber, handler);
}

template<typename TEvent>
void EventDispatcher::unsubscribeEvent(EventProcessor& subscriber)
{
    std::cout << "unsubscribeEvent() | subscriber " << subscriber.id();
    std::cout << " unsubscribed event " << eventID<TEvent>() << "\n";
    auto subscribers_it = m_subscribers.find(eventID<TEvent>());
    if (subscribers_it == std::end(m_subscribers)) return;

    subscribers_it->second.unsubscribe(subscriber);
}
#endif
