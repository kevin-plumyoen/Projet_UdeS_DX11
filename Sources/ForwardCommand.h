#ifndef FORWARDCOMMAND_H
#define FORWARDCOMMAND_H

#include "PlayerMovementCommand.h"

class ForwardCommand : public PlayerMovementCommand {
public:
    ForwardCommand(get_active_camera_controller get_camera_controller);
    Vec3 movement_direction() const noexcept override;
};

#endif