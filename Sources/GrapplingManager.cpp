#include "GrapplingManager.h"

#include "EventBus.h"
#include "GrapplingEvent.h"
#include "PhysicEngine.h"
#include "CameraManager.h"
#include "Logger.h"
#include "CollisionMasks.h"
#include "Transform.h"
#include "Renderer.h"
#include "GameplayCameraManager.h"

#include <extensions/PxJoint.h>

Mesh grappling_cable_mesh() {
    Mesh m;

    // Un cable est simplement un prisme triangulaire (6 vertex + 6 faces)
    m.vertices.resize(6);

    // Left
    m.indices.push_back(0);
    m.indices.push_back(5);
    m.indices.push_back(2);

    m.indices.push_back(0);
    m.indices.push_back(3);
    m.indices.push_back(5);

    // Bottom
    m.indices.push_back(0);
    m.indices.push_back(1);
    m.indices.push_back(4);
    
    m.indices.push_back(0);
    m.indices.push_back(4);
    m.indices.push_back(3);
    
    // Right
    m.indices.push_back(4);
    m.indices.push_back(2);
    m.indices.push_back(5);

    m.indices.push_back(4);
    m.indices.push_back(1);
    m.indices.push_back(2);

    return m;
}

GrapplingManager::GrapplingManager(Entity player, Entity camera, 
                                   PhysicEngine& physic, Renderer& renderer, 
                                   TransformStorage& t, UpdateStorage& u, CameraStorage& c, RigidBodyStorage& r,
                                   std::shared_ptr<Materiel> cable_material,
                                   PM3D::CDispositifD3D11* dispositif) noexcept
: physic{physic}
, renderer{renderer}
, update_components{u}
, camera_components{c}
, transform_components{t}
, rigidbody_components{r}
, cable_material{cable_material}
, player_entity{player}
, camera_manager_entity{camera} 
, left_grappling_joint{nullptr}
, right_grappling_joint{nullptr}
, left_grappling_mesh{grappling_cable_mesh()}
, right_grappling_mesh{grappling_cable_mesh()}
, left_grappling_mesh_renderer{left_grappling_mesh, dispositif}
, right_grappling_mesh_renderer{right_grappling_mesh, dispositif} {

}

GrapplingManager::~GrapplingManager() {
    if (left_grappling_joint) {
        physic.destroyJoint(left_grappling_joint);
    }

    if (right_grappling_joint) {
        physic.destroyJoint(right_grappling_joint);
    }
}

std::pair<Vec3, Vec3> GrapplingManager::joint_extremities(physx::PxJoint* joint) const noexcept {
    using namespace physx;

    PxRigidActor* actors[2] = {nullptr, nullptr};
    joint->getActors(actors[0], actors[1]);

    PxTransform actor_global_poses[2];
    std::transform(std::begin(actors), std::end(actors), std::begin(actor_global_poses), [](const PxRigidActor* actor) {
        return actor->getGlobalPose();
    });

    const PxTransform local_poses[2] = {
        joint->getLocalPose(PxJointActorIndex::eACTOR0),
        joint->getLocalPose(PxJointActorIndex::eACTOR1)
    };

    // Get joints extremities in world space with engine coordinates
    Vec3 positions[2];
    std::transform(std::begin(actor_global_poses), std::end(actor_global_poses), std::begin(local_poses), std::begin(positions), [](const PxTransform& global_pos, const PxTransform& local_pos) {
        const PxTransform world_transform = global_pos * local_pos;
      
        return PhysicEngine::positionPhysX2Motor(world_transform.p);
    });
    
    return std::make_pair(positions[0], positions[1]);
}

void GrapplingManager::update_grappling_mesh(Mesh& mesh, physx::PxJoint* joint) {
    using namespace DirectX;
    const std::pair<Vec3, Vec3> grappling_positions = joint_extremities(joint);

    const XMVECTOR pos_1_ = XMLoadFloat3(&grappling_positions.first);
    const XMVECTOR pos_2_ = XMLoadFloat3(&grappling_positions.second);
    const XMVECTOR delta_positions_ = pos_2_ - pos_1_;
    const XMVECTOR distance_ = XMVector3Length(delta_positions_);
    const XMVECTOR direction_ = XMVector3Normalize(delta_positions_);

    Vec3 direction;
    XMStoreFloat3(&direction, direction_);

    // TODO: create mesh to get cable between two extremities
    Transform cable_transform;
    cable_transform.look_at(direction);

    const float cable_radius = 0.15f;
    const Vec3 right = cable_transform.right();
    const Vec3 up = cable_transform.up();
    const XMVECTOR right_ = XMLoadFloat3(&right);
    const XMVECTOR up_ = XMLoadFloat3(&up);

    //const XMVECTOR c_ = pos_1_ + up_;
    const XMVECTOR quat_a_ = XMQuaternionRotationAxis(direction_, XMConvertToRadians(120));
    const XMVECTOR quat_b_ = XMQuaternionRotationAxis(direction_, XMConvertToRadians(240));
    const XMVECTOR a_dir_ = XMVector3Rotate(up_, quat_a_);
    const XMVECTOR b_dir_ = XMVector3Rotate(up_, quat_b_);

    const XMVECTOR pos_a1_ = pos_1_ + a_dir_ * cable_radius;
    const XMVECTOR pos_b1_ = pos_1_ + b_dir_ * cable_radius;
    const XMVECTOR pos_c1_ = pos_1_ + up_ * cable_radius;

    const XMVECTOR pos_a2_ = pos_2_ + a_dir_ * cable_radius;
    const XMVECTOR pos_b2_ = pos_2_ + b_dir_ * cable_radius;
    const XMVECTOR pos_c2_ = pos_2_ + up_ * cable_radius;

    const XMVECTOR directions_[6] = {
        a_dir_,
        b_dir_,
        up_,
        a_dir_,
        b_dir_,
        up_
    };

    const XMVECTOR positions_[6] = {
        pos_a1_,
        pos_b1_,
        pos_c1_,
        pos_a2_,
        pos_b2_,
        pos_c2_
    };

    for(int i = 0; i < 6; ++i) {
        XMStoreFloat3(&mesh.vertices[i].position, positions_[i]);
        XMStoreFloat3(&mesh.vertices[i].normal, directions_[i]);
    }

    // TODO: calculate uvs
}

void GrapplingManager::attach_grappling(physx::PxJoint*& grappling) {
    auto update_it = update_components.find(camera_manager_entity);
    if(update_it == update_components.end()) {
        log_critical("game.grappling_manager", "no camera manager attached to camera entity");
        return;
    }

    if (physic.is_connected_to_pvd()) {
        log_warning("game.grappling_manager", "cannot attached grappling because PVD is running");
        return;
    }

    GameplayCameraManager* camera_manager = static_cast<GameplayCameraManager*>(update_it->second.userdata);

    auto active_camera_it = camera_components.find(camera_manager->active_camera());
    if (active_camera_it == camera_components.end()) {
        log_critical("game.grappling_manager", "active camera entity not found");
        return;
    }

    const CameraRay ray = renderer.cameras()[active_camera_it->second.handle()].ray_from_viewport_coord(Vec2{0.f, 0.f});

    const DirectX::XMVECTOR ray_origin = DirectX::XMLoadFloat3(&ray.origin);
    const DirectX::XMVECTOR ray_dir = DirectX::XMLoadFloat3(&ray.direction);

    const DirectX::XMVECTOR ray_offset_origin = ray_origin + ray_dir * 5.f;
    Vec3 offset_position;
    DirectX::XMStoreFloat3(&offset_position, ray_offset_origin);

    const float max_cable_length = 100.f;

    auto raycast_result = physic.raycast(offset_position, ray.direction, max_cable_length, CollisionMasks::GrappableBit);
    if (raycast_result) {
        auto rigidbody_component_it = rigidbody_components.find(player_entity);
        assert(rigidbody_component_it != rigidbody_components.end());

        PxRigidActor* player_rigidbody = rigidbody_component_it->second.rigidbody;

        const Vec3 player_pos = PhysicEngine::positionPhysX2Motor(player_rigidbody->getGlobalPose().p);
        const Vec3 grappling_end_pos = raycast_result->position();

        const DirectX::XMVECTOR player_pos_ = DirectX::XMLoadFloat3(&player_pos);
        const DirectX::XMVECTOR grappling_end_pos_ = DirectX::XMLoadFloat3(&grappling_end_pos);
        const DirectX::XMVECTOR diff_ = grappling_end_pos_ - player_pos_;
        const DirectX::XMVECTOR diff_length_ = DirectX::XMVector3Length(diff_);

        float cable_length;
        DirectX::XMStoreFloat(&cable_length, diff_length_);

        PxRigidActor* to_attach_actor = raycast_result->actor();
        assert(player_rigidbody);
        assert(to_attach_actor);
                
        const PxVec3 px_hit_location = to_attach_actor->getGlobalPose().getInverse().transform(PhysicEngine::positionMotor2PhysX(raycast_result->position()));
        const Vec3 hit_location = PhysicEngine::positionPhysX2Motor(px_hit_location);

        grappling = physic.createDistanceJoint(player_rigidbody, Vec3{0.f, 0.f, 0.f},
                                               to_attach_actor, hit_location, cable_length);
        grappling->setConstraintFlag(physx::PxConstraintFlag::eCOLLISION_ENABLED, true);
    }
}

void GrapplingManager::detach_grappling(physx::PxJoint*& grappling) {
    grappling->release();
    grappling = nullptr;
}

void GrapplingManager::handle_event(const GrapplingEvent& ev, physx::PxJoint*& grappling) {
    if (ev.event == GrapplingEvent::Event::Attached && !grappling) {
        attach_grappling(grappling);
    }
    else if(ev.event == GrapplingEvent::Event::Detached && grappling) {
        detach_grappling(grappling);
    }
}

void GrapplingManager::update(FrameDuration) {
    for (const GrapplingEvent& ev : EventBus::instance().events<GrapplingEvent>()) {
        // Manages event on left grappling
        if (ev.grappling == GrapplingEvent::Grappling::Left) {
            handle_event(ev, left_grappling_joint);
        }
        // Manages event on right grappling
        else {
            handle_event(ev, right_grappling_joint);
        }
    }

    // Send draw commands
    if (left_grappling_joint) {
        update_grappling_mesh(left_grappling_mesh, left_grappling_joint);
        renderer.push_transparent_command(RenderObject3DCommand(&left_grappling_mesh_renderer, cable_material.get(), DirectX::XMMatrixIdentity()));
    }

    if (right_grappling_joint) {
        update_grappling_mesh(right_grappling_mesh, right_grappling_joint);
        renderer.push_transparent_command(RenderObject3DCommand(&right_grappling_mesh_renderer, cable_material.get(), DirectX::XMMatrixIdentity()));
    }
}