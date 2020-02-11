#ifndef DEF_SPLINE_FOLLOWING_SYSTEM_H
#define DEF_SPLINE_FOLLOWING_SYSTEM_H

#include "BaseTransformModifierSystem.h"
#include "ComponentStorages.h"

class SplineFollowingSystem : public BaseTransformModifierSystem {
    SplineStorage& splines;
public:
    SplineFollowingSystem(TransformStorage& transforms, TransformHierarchy& hierarchy, SplineStorage& splines);
    void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) override;
};

#endif