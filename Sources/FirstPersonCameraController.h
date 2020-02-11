#ifndef DEF_FIRST_PERSON_CAMERA_CONTROLLER_H
#define DEF_FIRST_PERSON_CAMERA_CONTROLLER_H

#include "BaseCameraController.h"

class FirstPersonCameraController : public BaseCameraController {
    Vec2 mouse_movement;
    float theta, phi;

    void update_angles_from_mouse_movement(FrameDuration dt) noexcept;
    DirectX::XMVECTOR direction_from_angle() const noexcept;
public:
    FirstPersonCameraController(Entity camera_entity, TransformStorage& transforms, TransformHierarchy& hierarchy);

    void handle_inputs(const PM3D::CDIManipulateur& input) override;
    void update(FrameDuration dt) override;
};

#endif