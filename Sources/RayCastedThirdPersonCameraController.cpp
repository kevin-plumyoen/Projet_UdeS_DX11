#include "RayCastedThirdPersonCameraController.h"

#include "CollisionMasks.h"
#include "PhysicEngine.h"
#include "PhysicHelpers.h"

#include <PxRigidActor.h>

RayCastedThirdPersonCameraController::RayCastedThirdPersonCameraController(Entity camera_entity, Entity target_entity, TransformStorage& transforms, TransformHierarchy& hierarchy, PhysicEngine& physic, float distance) 
: ThirdPersonCameraController(camera_entity, target_entity, transforms, hierarchy, distance)
, physic{physic} {

}

std::optional<Vec3> RayCastedThirdPersonCameraController::position_adjustment_in_environment(Vec3 new_position, Vec3 new_direction) const noexcept {
    auto res = physic.raycast(new_position, 
                              new_direction, 
                              distance(), 
                              CollisionMasks::CameraObstacleBit);

    if (res) {
        return res->position();
    }

    return {};
}

static Entity rigidbody_actor_owner(physx::PxRigidActor* actor) noexcept {
    const void* userdata = actor->userData;

    if(userdata) {
        intptr_t entity_bytes = reinterpret_cast<intptr_t>(userdata);
        --entity_bytes;
        Entity e;
        std::memcpy(&e, &entity_bytes, sizeof(Entity));

        return e;
    }

    return Entity{};
}

bool RayCastedThirdPersonCameraController::is_view_obstructed_by_target() const noexcept {
    auto res = physic.raycast(camera_transform().world.translation,
                              camera_transform().world.forward(),
                              distance(),
                              ~CollisionMasks::CameraObstacleBit);

    if (res) {
        Entity collided_actor = rigidbody_actor_owner(res->actor());

        return collided_actor == target();
    }

    return false;
}