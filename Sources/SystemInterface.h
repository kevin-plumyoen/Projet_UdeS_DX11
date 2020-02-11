#ifndef DEF_ECS_SYSTEM_INTERFACE_H
#define DEF_ECS_SYSTEM_INTERFACE_H

#include "EntityRegistry.h"
#include "Clock.h"

enum class ProcessState {
    Update,
    LateUpdate,
    PrePhysicUpdate,
    PostPhysicUpdate,
    PreRender,
    PostRender,
    StartOfFrame,
    EndOfFrame,
    HandleInputs
};

// All systems must implement this interface
struct BaseSystem {
    virtual ~BaseSystem() = default;
    virtual void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) = 0;
    virtual void cleanup() {}
};

#endif