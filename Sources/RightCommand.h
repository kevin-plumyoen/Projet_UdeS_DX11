#ifndef RIGHTCOMMAND_H
#define RIGHTCOMMAND_H

#include "PlayerMovementCommand.h"

class RightCommand : public PlayerMovementCommand {
public:
    RightCommand(get_active_camera_controller get_camera_controller);
    Vec3 movement_direction() const noexcept override;
};

#endif