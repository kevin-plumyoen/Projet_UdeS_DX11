#ifndef DEF_SIMPLE_CAMERA_CONTROLLER_H
#define DEF_SIMPLE_CAMERA_CONTROLLER_H

#include "ThirdPersonCameraController.h"
#include "MathHelpers.h"

class SimpleCameraController : public ThirdPersonCameraController {
    Vec2 mouse_movement;
public:
    SimpleCameraController(Entity camera, Entity target, TransformStorage& transforms, TransformHierarchy& hierarchy) noexcept;
    virtual ~SimpleCameraController() = default;

    void handle_inputs(const PM3D::CDIManipulateur& input) override;
    void update(FrameDuration dt) override;
};

#endif