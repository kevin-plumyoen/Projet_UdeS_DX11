#include "BackwardCommand.h"

#include "BaseCameraController.h"

BackwardCommand::BackwardCommand(get_active_camera_controller ctrl) 
: PlayerMovementCommand(ctrl, 10.f) {

}

Vec3 BackwardCommand::movement_direction() const noexcept {
    Vec3 backward{0.f, 0.f, 0.f};

    if(camera_controller()) {
        backward = camera_controller()->camera_transform().local.backward();
        backward.z = 0.f;
    }

    return backward;
}