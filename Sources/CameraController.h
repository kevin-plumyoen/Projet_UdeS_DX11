#ifndef DEF_GAME_CAMERA_CONTROLLER_H
#define DEF_GAME_CAMERA_CONTROLLER_H

#include "Clock.h"
#include "CatmullRom.h"
#include "Entity.h"
#include "ComponentStorages.h"
#include "RayCastedThirdPersonCameraController.h"

struct TransformComponent;
class Camera;
class PhysicEngine;

namespace PM3D {
    class CDIManipulateur;
}

class CameraController : public RayCastedThirdPersonCameraController {
    float theta_vel_;
    float phi_vel_;

    static const float max_phi;
    static const float min_phi;

    CatmullRom theta_curve;
    CatmullRom phi_curve;

    float theta_velocity(float x) const;
    float phi_velocity(float x) const;
    void reset_velocities() noexcept;
    void integrate_velocities(FrameDuration dt) noexcept;
    void update_camera_position() noexcept;

public:
    CameraController(Entity camera, Entity target, TransformStorage& transforms, TransformHierarchy& hierarchy, PhysicEngine& physic) noexcept;
    virtual ~CameraController() = default;

    void handle_inputs(const PM3D::CDIManipulateur& input) override;
    void update(FrameDuration dt) override;
};

#endif