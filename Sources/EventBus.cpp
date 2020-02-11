#include "EventBus.h"

EventBus& EventBus::instance() {
    static EventBus instance_;

    return instance_;
}

void EventBus::clear() {
    events_.clear();
    bytes_.clear();

    events_.swap(next_events_);
    bytes_.swap(next_bytes_);
}