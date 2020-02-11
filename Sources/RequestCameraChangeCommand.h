#ifndef DEF_REQUEST_CAMERA_CHANGE_COMMAND_H
#define DEF_REQUEST_CAMERA_CHANGE_COMMAND_H

#include "InputCommand.h"
#include "CameraEvent.h"

class RequestCameraChangeCommand : public InputCommand {
public:
    void execute(EntityProxy&, const FrameDuration&) override;
};

#endif