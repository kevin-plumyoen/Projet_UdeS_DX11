#include "KeyboardCameraChangeController.h"
#include "RequestCameraChangeCommand.h"
#include "DIManipulateur.h"

KeyboardCameraChangeController::KeyboardCameraChangeController(PM3D::CDIManipulateur* inputs)
: InputController{inputs}
, request_camera_change_command(std::make_unique<RequestCameraChangeCommand>())
, is_requesting_camera_change{false} {

}

void KeyboardCameraChangeController::getCommands() {
    if (inputs->ToucheAppuyee(ControllerLayout::get().change_camera) && !is_requesting_camera_change) {
        commands.push_back(request_camera_change_command.get());
        is_requesting_camera_change = true;
    }
    else if (!inputs->ToucheAppuyee(ControllerLayout::get().change_camera) && is_requesting_camera_change) {
        is_requesting_camera_change = false;
    }
}