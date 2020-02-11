#ifndef DEF_DEBUG_CAMERA_CONTROLLER_H
#define DEF_DEBUG_CAMERA_CONTROLLER_H

#include "Clock.h"

namespace PM3D {
    class CDIManipulateur;
}

class Camera;

class DebugCameraController {
    Camera& camera;
public:
    DebugCameraController(Camera& camera);

    void handle_input(const PM3D::CDIManipulateur& input, FrameDuration dt);

    void reset_to(const Camera& camera);
};

#endif