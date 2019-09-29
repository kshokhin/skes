#ifndef EVENT_SUBSCRIBERS_H
#define EVENT_SUBSCRIBERS_H

#include "EventProcessor.h"
#include "EventTraits.h"

class EventSubscribers
{
    using event_handler_t = std::function<void(const void*)>;
    using event_filter_t = std::function<bool(const void*)>;

    struct Subscription
    {
        subscriber_id_t subscriber_id = subscriber_id_t{};
        event_handler_t handler;
        event_filter_t filter;
        EventProcessor *publisher = 0;
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
            Subscription{subscriber.id(), (event_handler_t)handler_wrapper, event_filter_t{}});
    }

    template<typename TEvent>
    void subscribe(EventProcessor& subscriber,
        const typename Traits::event_handler_t<TEvent>& handler,
        const typename Traits::event_filter_t<TEvent>& filter)
    {
        auto handler_wrapper = [handler](const void* event)
        {
            const auto& typed_event = *((const TEvent*)event);
            handler(typed_event);
        };

        auto filter_wrapper = [filter](const void* event)
        {
            const auto& typed_event = *((const TEvent*)event);
            return filter(typed_event);
        };

        m_subscriptions.emplace_back(
            Subscription{subscriber.id(), (event_handler_t)handler_wrapper, (event_filter_t)filter_wrapper});
    }

    template<typename TEvent>
    void post(EventProcessor* publisher, const TEvent& event)
    {
        for (auto& subscription : m_subscriptions)
        {
            if (publisher != nullptr &&
                subscription.subscriber_id == publisher->id()) continue;

            if (subscription.filter && !subscription.filter((const void*)&event)) continue;

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

#endif