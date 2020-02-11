#ifndef DEF_ENGINE_EVENTS_H
#define DEF_ENGINE_EVENTS_H

#include "EventTraits.h"
#include "TriggerEvent.h"

/*
 * Cet événements est émis lorsque le jeu entre en pause ou lorsqu'il en sort
 */
struct PauseStateChangedEvent {
    bool is_paused;

    constexpr PauseStateChangedEvent(bool is_paused) noexcept
    : is_paused{ is_paused } {

    }
};

template<>
struct is_event<PauseStateChangedEvent> : std::true_type {};

struct EntityTriggeredEvent {
    Entity trigger;                                     // L'entité du trigger
    Entity entity;                                      // L'entité qui est entré dans le trigger
    TriggerEvent trigger_event;                         // Événement lié au trigger (on entre ou on sort?)
    uint32_t id;                                        // Id du trigger component
    void* userdata;                                     // Userdata du trigger component

    constexpr EntityTriggeredEvent(Entity trigger, TriggerEvent event, uint32_t id, void* userdata) noexcept
    : trigger{ trigger }
    , entity{ Entity() }
    , trigger_event{ event }
    , id{ id }
    , userdata{ userdata } {

    }

    constexpr EntityTriggeredEvent(Entity trigger, Entity entity, TriggerEvent event, uint32_t id, void* userdata) noexcept
    : trigger{ trigger }
    , entity{ entity }
    , trigger_event{ event }
    , id{ id }
    , userdata{ userdata } {

    }
};

template<>
struct is_event<EntityTriggeredEvent> : std::true_type {};

#endif