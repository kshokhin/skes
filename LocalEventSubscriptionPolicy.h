#ifndef LOCAL_EVENT_SUBSCRIBERS_POLICY_H
#define LOCAL_EVENT_SUBSCRIBERS_POLICY_H

#include <unordered_map>
#include <functional>
#include <vector>

#include "EventProcessor.h"
#include "EventTraits.h"

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
        const typename Traits::event_handler_t<TEvent>& handler)
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

class LocalEventSubscriptionPolicy
{
public:
    template<typename TEvent>
    void postEvent(size_t event_id, EventProcessor* publisher, const TEvent& event);

    template<typename TEvent>
    void subscribeEvent(size_t event_id, EventProcessor& subscriber,
        const typename Traits::event_handler_t<TEvent>& handler);

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
void LocalEventSubscriptionPolicy::unsubscribeEvent(
    size_t event_id, EventProcessor& subscriber)
{ 
    auto subscribers_it = m_subscribers.find(event_id);
    if (subscribers_it == std::end(m_subscribers)) return;

    subscribers_it->second.unsubscribe(subscriber);
}

#endif
