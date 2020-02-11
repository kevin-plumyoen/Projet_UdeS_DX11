#ifndef DEF_ECS_UPDATE_SYSTEM_H
#define DEF_ECS_UPDATE_SYSTEM_H

#include "SystemInterface.h"
#include "ComponentStorages.h"

#include <cstdint>

class UpdateSystem : public BaseSystem {
    UpdateStorage& updates;
    bool is_paused;

    void check_pause_state_changed_event();
public:
    UpdateSystem(UpdateStorage& updates) noexcept;
    void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) override;
};

#endif