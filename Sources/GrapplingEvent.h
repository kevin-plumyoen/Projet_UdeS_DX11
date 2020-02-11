#ifndef DEF_GRAPPLING_EVENT_H
#define DEF_GRAPPLING_EVENT_H

#include "EventTraits.h"
#include "MathHelpers.h"

struct GrapplingEvent {
    enum class Event : uint8_t {
        Attached,
        Detached
    };
    enum class Grappling : uint8_t {
        Right,
        Left
    };

    Event event;
    Grappling grappling;

    GrapplingEvent(Event event, Grappling grappling) noexcept
    : event{event} 
    , grappling{grappling} {

    }
};

template<>
struct is_event<GrapplingEvent> : std::true_type {};

#endif