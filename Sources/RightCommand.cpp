#include "RightCommand.h"

#include "BaseCameraController.h"

RightCommand::RightCommand(get_active_camera_controller ctrl)
: PlayerMovementCommand(ctrl, 10.f) {

}

Vec3 RightCommand::movement_direction() const noexcept {
    Vec3 direction{0.f, 0.f, 0.f};

    if(camera_controller()) {
        direction = camera_controller()->camera_transform().local.right();
        direction.z = 0.f;
    }

    return direction;
}