#include "SplineFollowingSystem.h"

SplineFollowingSystem::SplineFollowingSystem(TransformStorage& transforms, TransformHierarchy& hierarchy, SplineStorage& splines) 
: BaseTransformModifierSystem{transforms, hierarchy} 
, splines{splines} {

}

void SplineFollowingSystem::process(const EntityRegistry& entities, FrameDuration, ProcessState) {
    std::vector<Entity> entities_to_update;
    entities_to_update.reserve(1024);

    // Retrieves all entity with both components
    entities.entities_with(std::back_inserter(entities_to_update), transforms, splines);

    for (const Entity& e : entities_to_update) {
        auto spline_it = splines.find(e);
        auto transform_it = transforms.find(e);

        transform_it->second.world.translation = spline_it->second.spline.get(spline_it->second.t);

        // TODO: UPDATE ROTATION
        update_children_world_transforms(e);
    }
}