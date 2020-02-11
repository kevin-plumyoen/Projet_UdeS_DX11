#ifndef DEF_HUD_EVENT_H
#define DEF_HUD_EVENT_H

#include "EventTraits.h"
#include "MathHelpers.h"

struct HUDEvent {
    enum class Event : uint8_t {
        MenuOption,
        MenuScore,
        MenuPrecedent,
        MenuQuitter,
        MenuReprendre,
        MenuAfficher,
        OptionDesactiverBloom,
        OptionDesactiverRadialBlur,
        MenuAllerMenu,
        AfficherMenuFinDeJeux
    };

    Event event;

    HUDEvent(Event event) noexcept
        : event {event} {

    }
};

template<>
struct is_event<HUDEvent> : std::true_type {};

#endif