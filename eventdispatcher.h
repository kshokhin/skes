#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <optional>

#include "types.h"

template<typename TEvent>
struct EventHandlerType
{
    using type = std::function<void(const TEvent&)>;
};

class EventDispatcher;

using subscriber_id_t = size_t;

class EventProcessor
{
public:
    EventProcessor() {}
    virtual ~EventProcessor() = default;

    void setID(subscriber_id_t id) { m_id = id; }
    bool hasID() { return m_id.has_value(); }
    subscriber_id_t id() { return m_id.value(); }
private:
    std::optional<subscriber_id_t> m_id;
};

class EventSubscribers
{
    using event_handler_t = std::function<void(const void*)>;
    struct Subscription
    {
        subscriber_id_t subscriber_id = subscriber_id_t{};
        event_handler_t handler;
    };
public:
    template<typename TEvent>
    void subscribe(EventProcessor& subscriber,
        typename EventHandlerType<TEvent>::type handler)
    {
        auto handler_wrapper = [handler](const void* event)
        {
            const auto& typed_event = *((const TEvent*)event);
            handler(typed_event);
        };
        m_subscriptions.emplace_back(
            Subscription{subscriber.id(), (event_handler_t)handler_wrapper});
    }

    template<typename TEvent>
    void post(EventProcessor* publisher, const TEvent& event)
    {
        for (auto& subscription : m_subscriptions)
        {
            if (publisher != nullptr && 
                subscription.subscriber_id == publisher->id()) continue;

            subscription.handler((const void*)&event);
        }
    }

    void unsubscribe(EventProcessor& subscriber)
    {
        auto subscription_it = std::find_if(
            std::begin(m_subscriptions),
            std::end(m_subscriptions),
            [subscriber_id = subscriber.id()](const Subscription& subscription)
            {
                return subscription.subscriber_id == subscriber_id;
            });

        if (subscription_it == std::end(m_subscriptions)) return;
        m_subscriptions.erase(subscription_it);
    }
private:
    std::vector<Subscription> m_subscriptions;
};

class EventDispatcher
{
public:
    template<typename TEvent>
    void postEvent(EventProcessor* publisher, const TEvent& event);

    template<typename TEvent>
    void subscribeEvent(EventProcessor& subscriber,
            const typename EventHandlerType<TEvent>::type& handler);

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
void EventDispatcher::subscribeEvent(EventProcessor& subscriber, const typename EventHandlerType<TEvent>::type& handler)
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
