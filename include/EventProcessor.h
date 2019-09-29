#ifndef EVENT_PROCESSOR_H
#define EVENT_PROCESSOR_H

#include <cstdint>
#include <optional>

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

#endif
