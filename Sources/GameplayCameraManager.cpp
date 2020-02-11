#include "GameplayCameraManager.h"

#include "EventBus.h"
#include "CameraEvent.h"
#include "Logger.h"
#include "Profiler.h"
#include "PlayerEvent.h"
#include "EventBus.h"

#include <PxRigidActor.h>
#include <PxRigidDynamic.h>
#include <PxPhysicsAPI.h>

GameplayCameraManager::GameplayCameraManager(Entity fp_camera, Entity tp_camera, Entity player, TransformStorage& transforms, TransformHierarchy& hierarchy, CameraStorage& cameras, RigidBodyStorage& rigid_bodies) 
: transforms{transforms}
, hierarchy{hierarchy}
, cameras{cameras}
, rigid_bodies{rigid_bodies}
, fp_camera{fp_camera}
, tp_camera{tp_camera}
, player{player}
, state{State::Unknown} {

}

void GameplayCameraManager::detect_active_camera() {
    auto fp_camera_it = cameras.find(fp_camera);
    auto tp_camera_it = cameras.find(tp_camera);

    if (fp_camera_it == cameras.end()) {
        log_critical("game.gameplay_camera_manager", "First person camera entity doesn't have a camera component");
        throw std::runtime_error("invalid first person camera");
    }

    if (tp_camera_it == cameras.end()) {
        log_critical("game.gameplay_camera_manager", "Third person camera entity doesn't have a camera component");
        throw std::runtime_error("invalid third person camera");
    }

    if (tp_camera_it->second.active && fp_camera_it->second.active) {
        log_warning("game.gameplay_camera_manager", "Both cameras are marked as active, will choose the third person camera");
        fp_camera_it->second.active = false;
        state = State::ThirdPerson;
    }
    else if (tp_camera_it->second.active) {
        state = State::ThirdPerson;
    }
    else if (fp_camera_it->second.active) {
        state = State::FirstPerson;
    }
    else {
        log_warning("game.gameplay_camera_manager", "No cameras are marked as active, will choose the third person camera");
        tp_camera_it->second.active = true;
        state = State::ThirdPerson;
    }
}

void GameplayCameraManager::transit_to_first_person() {
    lock_rotation();

    auto fp_camera_it = cameras.find(fp_camera);
    auto fp_transform_it = transforms.find(fp_camera);

    auto tp_camera_it = cameras.find(tp_camera);
    auto tp_transform_it = transforms.find(tp_camera);

    const Vec3 tp_camera_pos = tp_transform_it->second.world.translation;
    const DirectX::XMVECTOR tp_camera_pos_ = DirectX::XMLoadFloat3(&tp_camera_pos);

    const Vec3 tp_forward = tp_transform_it->second.world.forward();
    const DirectX::XMVECTOR tp_forward_ = DirectX::XMLoadFloat3(&tp_forward);

    const DirectX::XMVECTOR tp_look_at_ = tp_camera_pos_ + tp_forward_;

    const Vec3 fp_position = fp_transform_it->second.world.translation;
    const DirectX::XMVECTOR fp_position_ = DirectX::XMLoadFloat3(&fp_position);

    const DirectX::XMVECTOR fp_inverse_direction_ = fp_position_ - tp_look_at_;
    const DirectX::XMVECTOR fp_look_at_ = fp_position_ + fp_inverse_direction_;

    // Make FP camera look at the same target then the TP camera
    Vec3 fp_camera_look_at;
    DirectX::XMStoreFloat3(&fp_camera_look_at, fp_look_at_);
    
    Transform new_fp_world_transform = fp_transform_it->second.world;
    new_fp_world_transform.look_at(fp_camera_look_at);

    hierarchy.update_world_transform(fp_camera, new_fp_world_transform);

    // Marque la caméra a activer
    fp_camera_it->second.active = true;
    tp_camera_it->second.active = false;

    EventBus::instance().transmit(PlayerRenderingEvent(PlayerRenderingEvent::Event::MakePlayerInvisible));

    state = State::FirstPerson;
}

void GameplayCameraManager::transit_to_third_person() {
    unlock_rotation();

    auto fp_camera_it = cameras.find(fp_camera);
    auto fp_transform_it = transforms.find(fp_camera);

    auto tp_camera_it = cameras.find(tp_camera);
    auto tp_transform_it = transforms.find(tp_camera);

    // TODO: Initialiser la caméra troisième personne pour qu'elle regarde dans la même direction
    EventBus::instance().transmit(PlayerRenderingEvent(PlayerRenderingEvent::Event::MakePlayerNormal));

    fp_camera_it->second.active = false;
    tp_camera_it->second.active = true;

    state = State::ThirdPerson;
}

void GameplayCameraManager::lock_rotation() {
    auto rigid_body_it = rigid_bodies.find(player);
    if (rigid_body_it != rigid_bodies.end()) {
        auto rigid_dynamic = rigid_body_it->second.rigidbody->is<physx::PxRigidDynamic>();
        rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
        rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
        rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

        auto global_pose = rigid_dynamic->getGlobalPose();
        global_pose.q = physx::PxQuat(physx::PxIDENTITY{});
        rigid_dynamic->setGlobalPose(global_pose);
    }
}

void GameplayCameraManager::unlock_rotation() {
    auto rigid_body_it = rigid_bodies.find(player);
    if (rigid_body_it != rigid_bodies.end()) {
        auto rigid_dynamic = rigid_body_it->second.rigidbody->is<physx::PxRigidDynamic>();
        rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, false);
        rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, false);
        rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, false);
    }
}

void GameplayCameraManager::update(FrameDuration) {
    if (state == State::Unknown) {
        detect_active_camera();
    }

    // Check if a camera change has been requested
    if (EventBus::instance().contains<CameraChangeEvent>()) {
        PROFILE_INSTANT_GLOBAL("camera changed", "gameplay");
        // A camera change has been requested
        switch (state) {
        case State::FirstPerson:
            transit_to_third_person();
            break;
        case State::ThirdPerson:
            transit_to_first_person();
            break;
        default:
            log_critical("game.gameplay_camera_manager", "manager is in invalid state");
            throw std::runtime_error("manager in invalid state");
        }
    }

    if (state == State::FirstPerson) {
        EventBus::instance().transmit(PlayerRenderingEvent(PlayerRenderingEvent::Event::MakePlayerInvisible));
    }
}

Entity GameplayCameraManager::active_camera() const {
    switch (state) {
    case State::FirstPerson:
        return fp_camera;
    case State::ThirdPerson:
        return tp_camera;
    default:
        return Entity{};
    }
}