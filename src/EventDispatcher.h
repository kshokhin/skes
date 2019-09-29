#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include "EventProcessor.h"
#include "EventTraits.h"

template<typename TEventSubscriptionPolicy>
class EventDispatcher : private TEventSubscriptionPolicy
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
};

template<typename TEventSubscriptionPolicy>
template<typename TEvent>
void EventDispatcher<TEventSubscriptionPolicy>::postEvent(
    EventProcessor* publisher, const TEvent& event)
{
    if (publisher != nullptr)
    {
        checkSubscriberID(*publisher);
    }

    std::cout << "postEvent() | " << "publisher ";
    std::cout << (publisher == nullptr ? "null" : std::to_string(publisher->id()));
    std::cout << " post event " << eventID<TEvent>() << "\n";

    TEventSubscriptionPolicy::template postEvent<TEvent>(
        eventID<TEvent>(), publisher, event);
}

template<typename TEventSubscriptionPolicy>
template<typename TEvent>
void EventDispatcher<TEventSubscriptionPolicy>::subscribeEvent(
    EventProcessor& subscriber,
    const typename Traits::event_handler_t<TEvent>& handler)
{
    checkSubscriberID(subscriber);
    std::cout << "subscribeEvent() | subscriber " << subscriber.id();
    std::cout << " subscribed to event " << eventID<TEvent>() << "\n";
    TEventSubscriptionPolicy::template subscribeEvent<TEvent>(
        eventID<TEvent>(), subscriber, handler);
}

template<typename TEventSubscriptionPolicy>
template<typename TEvent>
void EventDispatcher<TEventSubscriptionPolicy>::unsubscribeEvent(
    EventProcessor& subscriber)
{
    std::cout << "unsubscribeEvent() | subscriber " << subscriber.id();
    std::cout << " unsubscribed event " << eventID<TEvent>() << "\n";

    TEventSubscriptionPolicy::template unsubscribeEvent<TEvent>(
            eventID<TEvent>(), subscriber);
    
}
#endif
