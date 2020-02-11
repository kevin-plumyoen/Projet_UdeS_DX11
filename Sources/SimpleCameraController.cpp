#include "SimpleCameraController.h"
#include "Logger.h"
#include "DIManipulateur.h"

using namespace DirectX;

SimpleCameraController::SimpleCameraController(Entity camera, Entity target, TransformStorage& transforms, TransformHierarchy& hierarchy) noexcept
: ThirdPersonCameraController{ camera, target, transforms, hierarchy, 10.f } {

}

void SimpleCameraController::handle_inputs(const PM3D::CDIManipulateur& input) {
    mouse_movement = Vec2{static_cast<float>(input.EtatSouris().x()), 
                          static_cast<float>(input.EtatSouris().y())};
}

void SimpleCameraController::update(FrameDuration dt) {
    const float time = std::chrono::duration_cast<std::chrono::duration<float>>(dt).count();
    apply_angular_velocity(mouse_movement.x * time * 0.5f, mouse_movement.y * time * 0.5f);

    const Transform* target = target_transform();

    const XMVECTOR relative_position_vector_ = relative_camera_position();
    const XMVECTOR target_position_ = XMLoadFloat3(&target->translation);
    const XMVECTOR final_position_ = target_position_ + relative_position_vector_;

    Vec3 final_position;
    XMStoreFloat3(&final_position, final_position_);

    set_camera(final_position, target_transform()->translation);
}