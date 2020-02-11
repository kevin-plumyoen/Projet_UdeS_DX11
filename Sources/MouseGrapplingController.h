#ifndef DEF_MOUSE_GRAPPLING_CONTROLLER_H
#define DEF_MOUSE_GRAPPLING_CONTROLLER_H

#include "InputController.h"
#include "InputCommand.h"
#include <memory>

class MouseGrapplingController : public InputController {
    std::unique_ptr<InputCommand> attach_left_grappling_cmd;
    std::unique_ptr<InputCommand> attach_right_grappling_cmd;
    std::unique_ptr<InputCommand> detach_left_grappling_cmd;
    std::unique_ptr<InputCommand> detach_right_grappling_cmd;

    bool is_left_grappling_attached;
    bool is_right_grappling_attached;

    void attach_left_grappling();
    void detach_left_grappling();
    void attach_right_grappling();
    void detach_right_grappling();

public:
    MouseGrapplingController(PM3D::CDIManipulateur* direct_input);
    void getCommands() override;
};

#endif