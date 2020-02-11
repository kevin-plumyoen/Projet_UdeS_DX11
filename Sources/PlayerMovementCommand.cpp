#include "PlayerMovementCommand.h"
#include "EntityProxy.h"
#include "RigidBodyComponent.h"
#include "BaseCameraController.h"
#include "PhysicEngine.h"
#include <PxRigidDynamic.h>

#define MAX_SPEED 50.0f

using namespace physx;

PlayerMovementCommand::PlayerMovementCommand(get_active_camera_controller get_camera_controller, float speed)
: active_cam_controller_fn{get_camera_controller}
, speed_{speed} {

}

BaseCameraController* PlayerMovementCommand::camera_controller() noexcept {
    return active_cam_controller_fn();
}

const BaseCameraController* PlayerMovementCommand::camera_controller() const noexcept {
    return active_cam_controller_fn();
}

void PlayerMovementCommand::execute(EntityProxy& e, const FrameDuration& dt) {
    const float time = std::chrono::duration_cast<std::chrono::duration<float>>(dt).count();

    PxRigidDynamic* erd = reinterpret_cast<PxRigidDynamic*>(e.rigid_body->rigidbody);
    const PxVec3 direction_vector = PhysicEngine::positionMotor2PhysX(movement_direction());

    float final_speed = (erd->getLinearVelocity().magnitude() > MAX_SPEED) ? 0.0f : speed_;

    erd->setLinearVelocity(erd->getLinearVelocity() + direction_vector * time * final_speed);
}