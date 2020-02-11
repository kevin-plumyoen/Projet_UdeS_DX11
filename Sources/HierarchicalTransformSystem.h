#ifndef DEF_HIERARCHICAL_TRANSFORM_SYSTEM_H
#define DEF_HIERARCHICAL_TRANSFORM_SYSTEM_H

#include "Components.h"
#include "BaseTransformModifierSystem.h"

class HierarchicalTransformSystem : public BaseTransformModifierSystem {
public:
    HierarchicalTransformSystem(TransformStorage& transforms, TransformHierarchy& hierarchy);

    void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) override;
};

#endif