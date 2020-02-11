#include "BaseTransformModifierSystem.h"
#include "TransformHierarchy.h"

BaseTransformModifierSystem::BaseTransformModifierSystem(TransformStorage& transforms, TransformHierarchy& hierarchy) noexcept
: transforms{transforms}
, hierarchy{hierarchy} {

}

void BaseTransformModifierSystem::update_children_world_transforms(Entity e) {
    hierarchy.update_children_world_transforms(e);
}