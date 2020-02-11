#include "ThirdPersonCameraController.h"
#include "TransformHierarchy.h"
#include "Logger.h"

const float ThirdPersonCameraController::min_phi_ = DirectX::XMConvertToRadians(-60.f);
const float ThirdPersonCameraController::max_phi_ = DirectX::XMConvertToRadians(85.f);

ThirdPersonCameraController::ThirdPersonCameraController(Entity camera_entity, Entity target_entity, TransformStorage& transforms, TransformHierarchy& hierarchy, float distance) 
: BaseCameraController(camera_entity, transforms, hierarchy)
, target_entity{target_entity}
, current_phi_{0.f}
, current_theta_{0.f}
, camera_distance_{distance} {
    if (!target_entity.valid()) {
        log_warning("third_Person_camera_controller_ctor", "invalid target entity");
    }
}

void ThirdPersonCameraController::apply_angular_velocity(float theta_vel, float phi_vel) {
    current_theta_ += theta_vel;
    current_phi_ += phi_vel;

    if (current_phi_ < min_phi_) {
        current_phi_ = min_phi_;
    }
    else if (current_phi_ > max_phi_) {
        current_phi_ = max_phi_;
    }
}

void ThirdPersonCameraController::set_camera(const Vec3& position, const Vec3& target) {
    auto camera_transform_it = transforms().find(camera_entity());
   
    if (camera_transform_it == transforms().end()) {
        log_warning("game.third_persone_camera_controller", "camera has no transform");
        return;
    }

    Transform camera_transform = camera_transform_it->second.world;
    camera_transform.translation = position;
    camera_transform.look_at(target);

    hierarchy().update_world_transform(camera_entity(), std::move(camera_transform));
}

float ThirdPersonCameraController::phi() const noexcept {
    return current_phi_;
}

float ThirdPersonCameraController::theta() const noexcept {
    return current_theta_;
}

float ThirdPersonCameraController::distance() const noexcept {
    return camera_distance_;
}

DirectX::XMVECTOR ThirdPersonCameraController::relative_camera_position() const noexcept {
    return DirectX::XMVectorSet(distance() * DirectX::XMScalarCos(theta()) * DirectX::XMScalarCos(phi()),
                                distance() * DirectX::XMScalarSin(theta()) * DirectX::XMScalarCos(phi()),
                                distance() * DirectX::XMScalarSin(phi()),
                                1.0f);
}

const Transform* ThirdPersonCameraController::target_transform() const noexcept {
    auto target_transform_it = transforms().find(target_entity);
   
    if (target_transform_it == transforms().end()) {
        log_warning("game.third_persone_camera_controller", "target has no transform");
        return nullptr;
    }

    return &target_transform_it->second.world;
}

Entity ThirdPersonCameraController::target() const noexcept {
    return target_entity;
}