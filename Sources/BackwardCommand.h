#ifndef BACKWARDCOMMAND_H
#define BACKWARDCOMMAND_H

#include "InputCommand.h"
#include "PlayerMovementCommand.h"

class BackwardCommand : public PlayerMovementCommand {
public:
    BackwardCommand(get_active_camera_controller get_camera_controller);
    Vec3 movement_direction() const noexcept override;
};

#endif