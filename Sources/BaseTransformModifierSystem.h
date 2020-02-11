#ifndef DEF_BASE_TRANSFORM_MODIFIER_SYSTEM_H
#define DEF_BASE_TRANSFORM_MODIFIER_SYSTEM_H

#include "SystemInterface.h"
#include "ComponentStorages.h"

class TransformHierarchy;

class BaseTransformModifierSystem : public BaseSystem {
protected:
    TransformStorage& transforms;
    TransformHierarchy& hierarchy;

    void update_children_world_transforms(Entity e);

public:
    BaseTransformModifierSystem(TransformStorage& transforms, TransformHierarchy& hierarchy) noexcept;

};

#endif