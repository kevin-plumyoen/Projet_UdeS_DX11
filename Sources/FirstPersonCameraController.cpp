#include "FirstPersonCameraController.h"
#include "DIManipulateur.h"

#include <algorithm>

static float max_phi =  1.309f;
static float min_phi = -1.309f;

FirstPersonCameraController::FirstPersonCameraController(Entity camera_entity, TransformStorage& transforms, TransformHierarchy& hierarchy)
: BaseCameraController(camera_entity, transforms, hierarchy) 
, mouse_movement{0.f, 0.f}
, theta{0.f}
, phi{0.f} {

}

void FirstPersonCameraController::update_angles_from_mouse_movement(FrameDuration dt) noexcept {
    const float time_sec = std::chrono::duration_cast<std::chrono::duration<float>>(dt).count();

    theta += mouse_movement.x * time_sec;
    phi = std::clamp(phi - mouse_movement.y * time_sec, min_phi, max_phi);
}

void FirstPersonCameraController::handle_inputs(const PM3D::CDIManipulateur& input) {
    mouse_movement = Vec2(static_cast<float>(input.EtatSouris().x()), 
                          static_cast<float>(input.EtatSouris().y()));
}

DirectX::XMVECTOR FirstPersonCameraController::direction_from_angle() const noexcept {
    return DirectX::XMVectorSet(DirectX::XMScalarCos(theta) * DirectX::XMScalarCos(phi),
                                DirectX::XMScalarSin(theta) * DirectX::XMScalarCos(phi),
                                DirectX::XMScalarSin(phi),
                                1.0f);
}

void FirstPersonCameraController::update(FrameDuration dt) {
    update_angles_from_mouse_movement(dt);

    const DirectX::XMVECTOR direction_ = direction_from_angle();
    Vec3 direction;
    DirectX::XMStoreFloat3(&direction, direction_);

    camera_transform().local.look_at(direction);
}