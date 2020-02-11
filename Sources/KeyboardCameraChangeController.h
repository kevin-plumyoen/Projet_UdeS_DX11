#ifndef DEF_KEYBOARD_CAMERA_CHANGE_CONTROLLER_H
#define DEF_KEYBOARD_CAMERA_CHANGE_CONTROLLER_H

#include "InputController.h"
#include "InputCommand.h"
#include <memory>

class KeyboardCameraChangeController : public InputController {
    std::unique_ptr<InputCommand> request_camera_change_command;
    bool is_requesting_camera_change;
public:
    KeyboardCameraChangeController(PM3D::CDIManipulateur* inputs);

    void getCommands() override;
};

#endif