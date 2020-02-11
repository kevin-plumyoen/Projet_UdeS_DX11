#ifndef DEF_THIRD_PERSON_CAMERA_CONTROLLER_H
#define DEF_THIRD_PERSON_CAMERA_CONTROLLER_H

#include "BaseCameraController.h"
#include "Entity.h"

class ThirdPersonCameraController : public BaseCameraController {
    Entity target_entity;
    float current_theta_;
    float current_phi_;
    float camera_distance_;
    static const float max_phi_;
    static const float min_phi_;
protected:
    void apply_angular_velocity(float theta_vel, float phi_vel);
    void set_camera(const Vec3& position, const Vec3& target);

    DirectX::XMVECTOR relative_camera_position() const noexcept;
public:
    ThirdPersonCameraController(Entity camera_entity, Entity target_entity, TransformStorage& transforms, TransformHierarchy& hierarchy, float distance);

    float phi() const noexcept;
    float theta() const noexcept;
    float distance() const noexcept;

    const Transform* target_transform() const noexcept;
    Entity target() const noexcept;
};

#endif