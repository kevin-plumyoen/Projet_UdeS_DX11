#include "LeftCommand.h"

#include "BaseCameraController.h"

LeftCommand::LeftCommand(get_active_camera_controller ctrl)
: PlayerMovementCommand(ctrl, 10.f) {

}

Vec3 LeftCommand::movement_direction() const noexcept {
    Vec3 direction{0.f, 0.f, 0.f};

    if(camera_controller()) {
        direction = camera_controller()->camera_transform().local.left();
        direction.z = 0.f;
    }

    return direction;
}