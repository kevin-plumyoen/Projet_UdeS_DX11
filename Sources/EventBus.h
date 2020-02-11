#ifndef DEF_EVENT_BUS_H
#define DEF_EVENT_BUS_H

#include "EventTraits.h"
#include <vector>
#include <typeinfo>
#include <cassert>

class EventBus {
public:
    struct Event {
        std::size_t type;
        std::size_t data_offset;

        constexpr Event(std::size_t type, std::size_t offset) noexcept
            : type{ type }
            , data_offset{ offset } {

        }
    };
private:
    std::vector<Event> events_;
    std::vector<char> bytes_;
    std::vector<Event> next_events_;
    std::vector<char> next_bytes_;
    EventBus() = default;
public:
    struct SearchFirstType{};

    template<typename EventType>
    class EventIterator {
        static_assert(is_event_v<EventType>, "invalid event type");
        friend EventBus;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = EventType;
        using difference_type = std::size_t;
        using pointer = const EventType*;
        using reference = const EventType&;
    private:
        std::size_t index = 0;
        const std::vector<Event>* events = nullptr;
        const std::vector<char>* bytes = nullptr;

        EventIterator(const std::vector<Event>* events, const std::vector<char>* data, std::size_t index) noexcept
        : index{ index }
        , events{ events }
        , bytes{ data } {

        }

        EventIterator(const std::vector<Event>* events, const std::vector<char>* data, std::size_t index, SearchFirstType) noexcept
        : index{ index }
        , events{ events }
        , bytes{ data } {
            // Search first event of this type
            if (index < events->size() 
            && (*events)[index].type != typeid(EventType).hash_code()) {
                ++(*this);
            }
        }

        pointer extract_event() const noexcept {
            assert(events);
            assert(index < events->size());
            assert(bytes);

            const std::size_t data_offset = (*events)[index].data_offset;
            const char* raw_bytes = &(*bytes)[data_offset];

            return reinterpret_cast<const EventType*>(raw_bytes);
        }
    public:
        EventIterator& operator++() noexcept {
            assert(events);
            assert(index < events->size());
            assert(bytes);

            const std::size_t event_type_id = typeid(EventType).hash_code();

            do {
                ++index;
            } while (index < events->size() 
                 && (*events)[index].type != event_type_id);

            return *this;
        }

        EventIterator operator++(int) noexcept {
            EventIterator old = *this;

            ++(*this);

            return old;
        }

        EventIterator& operator--() noexcept {
            assert(events);
            assert(bytes);
            assert(index > 0);

            do {
                --index;
            } while(index > 0 && (*events)[index].type == typeid(EventType).hash_code());

            return *this;
        }

        EventIterator operator--(int) noexcept {
            EventIterator old = *this;

            --(*this);

            return old;
        }

        bool operator==(const EventIterator& other) const noexcept {
            return events == other.events && bytes == other.bytes && index == other.index;
        }

        bool operator!=(const EventIterator& other) const noexcept {
            return events != other.events || bytes != other.bytes || index != other.index;
        }

        pointer operator->() const noexcept {
            return extract_event();
        }

        reference operator*() const noexcept {
            return *extract_event();
        }
    };

    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    static EventBus& instance();
    void clear();

    template<typename EventType,
    typename = std::enable_if_t<is_event_v<EventType>>>
    void transmit(const EventType& ev) {
        static_assert(std::is_trivially_copyable_v<EventType>, "EventType must be trivially copyable");

        const char* raw_bytes = reinterpret_cast<const char*>(&ev);
        const std::size_t offset = bytes_.size();

        std::copy(raw_bytes, raw_bytes + sizeof(EventType),
                  std::back_inserter(bytes_));

        events_.emplace_back(typeid(EventType).hash_code(), offset);
    }

    template<typename EventType,
             typename = std::enable_if_t<is_event_v<EventType>>>
    void transmit_next_frame(const EventType& ev) {
        static_assert(std::is_trivially_copyable_v<EventType>, "EventType must be trivially copyable");

        const char* raw_bytes = reinterpret_cast<const char*>(&ev);
        const std::size_t offset = next_bytes_.size();

        std::copy(raw_bytes, raw_bytes + sizeof(EventType),
                  std::back_inserter(next_bytes_));

        next_events_.emplace_back(typeid(EventType).hash_code(), offset);
    }

    template<typename EventType>
    class EventsRange {
        friend EventBus;

        const std::vector<Event>* events;
        const std::vector<char>* data;

        EventsRange(const std::vector<Event>* events, const std::vector<char>* data) noexcept
        : events{ events }
        , data{ data } {

        }

    public:
        EventIterator<EventType> begin() const noexcept {
            return EventIterator<EventType>{events, data, 0, SearchFirstType{}};
        }

        EventIterator<EventType> end() const noexcept {
            return EventIterator<EventType>{events, data, events->size()};
        }
    };

    template<typename EventType>
    EventsRange<EventType> events() const noexcept {
        return EventsRange<EventType>(&events_, &bytes_);
    }

    template<typename EventType>
    bool contains() const noexcept {
        return events<EventType>().begin() != events<EventType>().end();
    }
};

#endif