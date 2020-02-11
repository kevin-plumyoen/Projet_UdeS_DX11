#ifndef DEF_PLAYER_EVENT_H
#define DEF_PLAYER_EVENT_H

#include "EventTraits.h"
#include "MathHelpers.h"

struct PlayerRenderingEvent {
    enum class Event : uint8_t {
        MakePlayerInvisible,
        MakePlayerTransparent,
        MakePlayerNormal
    };

    Event event;

    PlayerRenderingEvent(Event event) noexcept
    : event{event} {

    }
};

template<>
struct is_event<PlayerRenderingEvent> : std::true_type {};


#endif