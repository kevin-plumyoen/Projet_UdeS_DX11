#include "UpdateRigidBodyTransformsSystem.h"
#include "TransformHierarchy.h"

#include <PxRigidActor.h>
#include <../PhysicEngine.h>

UpdateRigidBodyTransformsSystem::UpdateRigidBodyTransformsSystem(TransformStorage& transforms, TransformHierarchy& hierarchy, const RigidBodyStorage& rigidbodies)
: BaseTransformModifierSystem{ transforms, hierarchy }
, rigidbodies{ rigidbodies } {

}

void UpdateRigidBodyTransformsSystem::process(const EntityRegistry& entities, FrameDuration, ProcessState) {
    std::vector<Entity> entities_to_update;
    entities_to_update.reserve(1024);

    // Retrieves all entity with both components
    entities.entities_with(std::back_inserter(entities_to_update), transforms, rigidbodies);

    for (const Entity& e : entities_to_update) {
        auto transform_it = transforms.find(e);
        auto rigidbody_it = rigidbodies.find(e);
        if (rigidbody_it->second.updateTransform) {
            // Update transform from rigidbody
            const physx::PxTransform global_pose = rigidbody_it->second.rigidbody->getGlobalPose();

            // Update world transform
            transform_it->second.world.translation = PhysicEngine::positionPhysX2Motor(global_pose.p);
            transform_it->second.world.rotation = PhysicEngine::rotationPhysX2Motor(global_pose.q);

            const bool has_parent = entities.is_child(e);
            auto parent_transform = has_parent ? transforms.find(entities.parent_of(e)) : transforms.end();
            const bool parent_has_transform = parent_transform != transforms.end();

            if (has_parent && parent_has_transform) {
                transform_it->second.local = TransformHierarchy::apply_parent_inverse_transform(parent_transform->second.world, transform_it->second.world);
            }
            else {
                transform_it->second.local = transform_it->second.world;
            }

            update_children_world_transforms(e);
        }
    }
}