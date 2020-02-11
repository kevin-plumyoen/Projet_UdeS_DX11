#ifndef DEF_PLAYER_MOVEMENT_COMMAND_H
#define DEF_PLAYER_MOVEMENT_COMMAND_H

#include "MathHelpers.h"
#include "Clock.h"
#include "InputCommand.h"
#include <functional>

class BaseCameraController;
struct EntityProxy;

class PlayerMovementCommand : public InputCommand {
public:
    using get_active_camera_controller = std::function<BaseCameraController*()>;
private:
    get_active_camera_controller active_cam_controller_fn;
    float speed_;

    virtual Vec3 movement_direction() const noexcept = 0;
protected:
    BaseCameraController* camera_controller() noexcept;
    const BaseCameraController* camera_controller() const noexcept;
public:
    PlayerMovementCommand(get_active_camera_controller get_camera_controller, float speed);

    void execute(EntityProxy& e, const FrameDuration& dt) override;
};

#endif