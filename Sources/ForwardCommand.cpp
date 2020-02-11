#include "ForwardCommand.h"

#include "BaseCameraController.h"

ForwardCommand::ForwardCommand(get_active_camera_controller ctrl)
: PlayerMovementCommand(ctrl, 10.f) {

}

Vec3 ForwardCommand::movement_direction() const noexcept {
    Vec3 forward{0.f, 0.f, 0.f};

    if(camera_controller()) {
        forward = camera_controller()->camera_transform().local.forward();
        forward.z = 0.f;
    }

    return forward;
}