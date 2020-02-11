#include "UpdateSystem.h"
#include "EngineEvents.h"
#include "EventBus.h"

UpdateSystem::UpdateSystem(UpdateStorage& updates) noexcept
: updates{updates}
, is_paused{false} {

}

void UpdateSystem::check_pause_state_changed_event() {
    for (const PauseStateChangedEvent& ev : EventBus::instance().events<PauseStateChangedEvent>()) {
        is_paused = ev.is_paused;
    }
}

void UpdateSystem::process(const EntityRegistry& entities, FrameDuration dt, ProcessState state) {
    check_pause_state_changed_event();

    std::vector<Entity> entities_to_update;
    entities_to_update.reserve(16);

    // Retrieves all entity with both components
    entities.entities_with(std::back_inserter(entities_to_update), updates);
    
    uint8_t update_mask = 0;
    if (state == ProcessState::LateUpdate) {
        update_mask |= UpdateFlags::LateUpdateBit;
    }
    else if (state == ProcessState::Update) {
        update_mask |= UpdateFlags::UpdateBit;
    }

    if (is_paused) {
        update_mask |= UpdateFlags::ExecuteWhenPausedBit;
    }

    for (const Entity& e : entities_to_update) {
        auto update_it = updates.find(e);

        if ((update_it->second.flags & update_mask) == update_mask) {
            update_it->second.callback(dt, update_mask, update_it->second.userdata);
        }
    }
}