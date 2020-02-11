#include "CameraController.h"

#include "DIManipulateur.h"
#include "TransformComponent.h"
#include "MathHelpers.h"
#include "Camera.h"
#include "PhysicEngine.h"
#include "ComponentStorages.h"
#include "Logger.h"
#include "CollisionMasks.h"
#include <DirectXMath.h>
#include <cmath>
#include <algorithm>

#include <chrono>

#undef max
#undef min

using namespace std::chrono;
const float CameraController::min_phi = DirectX::XMConvertToRadians(-15.f);
const float CameraController::max_phi = DirectX::XMConvertToRadians(60.f);

CameraController::CameraController(Entity camera, Entity target, TransformStorage& transforms, TransformHierarchy& hierarchy, PhysicEngine& physic) noexcept 
: RayCastedThirdPersonCameraController{camera, target, transforms, hierarchy, physic, 30.f} 
, theta_vel_{0.f}
, phi_vel_{0.f} {
    theta_curve.add(-1.f, 6.28318531f) // 360 + 180
               .add(-1.f, 6.28318531f) // 360 + 180
               .add(-0.25f, 0.f)
               .add(0.f, -1.f)
               .add(0.25f, 0.f)
               .add(1.f, 6.28318531f) // 360 + 180
               .add(1.f, 6.28318531f); // 360 + 180

    phi_curve.add(-1.f, 3.14159f)
             .add(-1.f, 3.14159f)
             .add(-0.5f, 0.f)
             .add(0.f, -1.f)
             .add(0.5f, 0.f)
             .add(1.f, 3.14159f)
             .add(1.f, 3.14159f);
}

void CameraController::handle_inputs(const PM3D::CDIManipulateur& input) {
    const Vec2 normalized_coords = input.normalized_mouse_coords();

    theta_vel_ = theta_velocity(normalized_coords.x);
    phi_vel_ = phi_velocity(-normalized_coords.y);
}

float CameraController::theta_velocity(float x) const {
    const float theta_velocity = *theta_curve.get(x);
    const float theta_velocity_sign = x < 0.f ? -1.f : 1.f;

    return theta_velocity_sign * std::max(0.f, theta_velocity);
}

float CameraController::phi_velocity(float x) const {
    const float phi_velocity = *phi_curve.get(x);
    const float phi_velocity_sign = x < 0.f ? -1.f : 1.f;

    return phi_velocity_sign * std::max(0.f, phi_velocity);
}

void CameraController::reset_velocities() noexcept {
    theta_vel_ = 0.f;
    phi_vel_ = 0.f;
}

void CameraController::integrate_velocities(FrameDuration dt) noexcept {
    const auto seconds_dt = std::chrono::duration_cast<std::chrono::duration<float>>(dt);

    apply_angular_velocity(theta_vel_ * seconds_dt.count(), phi_vel_ * seconds_dt.count());
}

void CameraController::update_camera_position() noexcept {
    using namespace DirectX;

    //============================================================================================
    // Position without environment
    //============================================================================================
    const XMVECTOR relative_position_vector = relative_camera_position();
    const XMVECTOR camera_offset_direction = XMVector3Normalize(relative_position_vector);

    const XMVECTOR target_position = XMLoadFloat3(&target_transform()->translation);
    const XMVECTOR camera_position = target_position + relative_position_vector;
    XMVECTOR camera_look_direction = target_position - camera_position;
    Vec3 camera_look_direction_z;
    XMStoreFloat3(&camera_look_direction_z, camera_look_direction);
    camera_look_direction_z.z = 0.f;
    camera_look_direction= XMVector3Normalize(XMLoadFloat3(&camera_look_direction_z));

    Vec3 camera_position_;
    XMStoreFloat3(&camera_position_, camera_position);

    Vec3 camera_offset_direction_;
    XMStoreFloat3(&camera_offset_direction_, camera_offset_direction);

    Vec3 camera_look_direction_;
    XMStoreFloat3(&camera_look_direction_, camera_look_direction);

    const float raycast_distance = distance() * 0.9f;

    //============================================================================================
    // Verify with environment
    //============================================================================================
    auto adapted_position = position_adjustment_in_environment(target_transform()->translation, camera_offset_direction_);
    if (adapted_position) {
        Vec3 new_position = *adapted_position;
        set_camera(new_position, target_transform()->translation);
    }
    else {
        set_camera(camera_position_, target_transform()->translation);
    }
}

void CameraController::update(FrameDuration dt) {
    integrate_velocities(dt);
    update_camera_position();
    reset_velocities();
}

