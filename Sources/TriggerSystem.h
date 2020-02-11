#ifndef DEF_TRIGGER_SYSTEM_H
#define DEF_TRIGGER_SYSTEM_H

#include "SystemInterface.h"
#include "ComponentStorages.h"

class PhysicEngine;

class TriggerSystem : public BaseSystem {
    const PhysicEngine& physic;
    const TriggerStorage& triggers;
    const RigidBodyStorage& rigidbodies;
public:
    TriggerSystem(const PhysicEngine& physic, const TriggerStorage& triggers, const RigidBodyStorage& rigidbodies) noexcept;
    void process(const EntityRegistry&, FrameDuration, ProcessState) override;
};

#endif