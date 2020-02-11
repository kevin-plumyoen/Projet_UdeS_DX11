#ifndef DEF_ECS_UPDATE_RIGIDBODY_TRANSFORMS_SYSTEM_H
#define DEF_ECS_UPDATE_RIGIDBODY_TRANSFORMS_SYSTEM_H

#include "BaseTransformModifierSystem.h"
#include "ComponentStorages.h"

class UpdateRigidBodyTransformsSystem : public BaseTransformModifierSystem {
    const RigidBodyStorage& rigidbodies;

public:
    UpdateRigidBodyTransformsSystem(TransformStorage& transforms, TransformHierarchy& hierarchy, const RigidBodyStorage& rigidbodies);
    virtual ~UpdateRigidBodyTransformsSystem() = default;
    void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) override;
};

#endif