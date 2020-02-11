#ifndef LEFTCOMMAND_H
#define LEFTCOMMAND_H

#include "PlayerMovementCommand.h"

class LeftCommand : public PlayerMovementCommand {
public:
    LeftCommand(get_active_camera_controller get_camera_controller);
    Vec3 movement_direction() const noexcept override;
};

#endif