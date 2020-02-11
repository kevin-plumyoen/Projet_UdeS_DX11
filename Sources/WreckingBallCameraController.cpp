#include "WreckingBallCameraController.h"
#include "DIManipulateur.h"
#include "Logger.h"
#include "EventBus.h"
#include "PlayerEvent.h"

using namespace DirectX;

WreckingBallCameraController::WreckingBallCameraController(Entity camera, Entity target, TransformStorage& transforms, TransformHierarchy& hierarchy, PhysicEngine& physic) noexcept
: RayCastedThirdPersonCameraController(camera, target, transforms, hierarchy, physic, 10.0f)
, meshes{meshes} {


}

void WreckingBallCameraController::handle_inputs(const PM3D::CDIManipulateur& input) {
    mouse_movement = Vec2(static_cast<float>(input.EtatSouris().x()), 
                          static_cast<float>(input.EtatSouris().y()));
}

void WreckingBallCameraController::find_optimal_position() {
    const Transform* target = target_transform();

    const XMVECTOR target_position_ = XMLoadFloat3(&target->translation);
    const XMVECTOR base_cam_relative_position_vector_ = relative_camera_position();
    const XMVECTOR optimal_cam_position_ = target_position_ + base_cam_relative_position_vector_;

    const XMVECTOR cam_offset_direction_ = XMVector3Normalize(optimal_cam_position_ - target_position_);
    const XMVECTOR vertical_cam_offset_scalar_ = XMVector3Dot(cam_offset_direction_, XMVectorSet(0.f, 0.f, 1.f, 0.f));
    float vertical_cam_offset_scalar;
    XMStoreFloat(&vertical_cam_offset_scalar, vertical_cam_offset_scalar_);
    const XMVECTOR cam_horizontal_offset_direction_ = cam_offset_direction_ - XMVectorSet(0.f, 0.f, vertical_cam_offset_scalar, 0.f);
    
    const XMVECTOR offseted_target_positions_ = target_position_ - 2.f * cam_horizontal_offset_direction_ + XMVectorSet(0.f, 0.f, 1.f, 0.f);

    Vec3 optimal_cam_position;
    Vec3 cam_offset_direction;
    Vec3 target_position;

    XMStoreFloat3(&optimal_cam_position, optimal_cam_position_);
    XMStoreFloat3(&cam_offset_direction, cam_offset_direction_);
    XMStoreFloat3(&target_position, offseted_target_positions_);

    auto adapted_position = position_adjustment_in_environment(target_transform()->translation, cam_offset_direction);
    if (adapted_position) {
        Vec3 new_position = *adapted_position;
        set_camera(new_position, target_position);
    }
    else {
        set_camera(optimal_cam_position, target_position);
    }
}

void WreckingBallCameraController::handle_player_view_obstruction() {
    if (is_view_obstructed_by_target()) {
        EventBus::instance().transmit_next_frame(PlayerRenderingEvent{PlayerRenderingEvent::Event::MakePlayerTransparent});
    }
    else {
        EventBus::instance().transmit_next_frame(PlayerRenderingEvent{PlayerRenderingEvent::Event::MakePlayerNormal});
    }
}

void WreckingBallCameraController::update(FrameDuration dt) {
    const float time = std::chrono::duration_cast<std::chrono::duration<float>>(dt).count();
    apply_angular_velocity(mouse_movement.x * time * 0.5f, mouse_movement.y * time * 0.5f);
    find_optimal_position();
    
    handle_player_view_obstruction();
}