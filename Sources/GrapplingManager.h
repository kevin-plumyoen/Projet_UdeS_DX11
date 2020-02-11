#ifndef DEF_GAME_GRAPPLING_MANAGER_H
#define DEF_GAME_GRAPPLING_MANAGER_H

#include "Clock.h"
#include "Entity.h"
#include "ComponentStorages.h"
#include "Mesh.h"
#include "DynamicMesh.h"

#include <memory>

class PhysicEngine;
class CameraManager;
struct GrapplingEvent;
struct Renderer;
class Material;

namespace physx {
    class PxJoint;
}

class GrapplingManager {
    PhysicEngine& physic;
    Renderer& renderer;
    TransformStorage& transform_components;
    UpdateStorage& update_components;
    CameraStorage& camera_components;
    RigidBodyStorage& rigidbody_components;
    std::shared_ptr<Materiel> cable_material;
    Entity player_entity;
    Entity camera_manager_entity;

    physx::PxJoint* left_grappling_joint;
    physx::PxJoint* right_grappling_joint;

    Mesh left_grappling_mesh;
    Mesh right_grappling_mesh;
    DynamicMesh left_grappling_mesh_renderer;
    DynamicMesh right_grappling_mesh_renderer;

    std::pair<Vec3, Vec3> joint_extremities(physx::PxJoint* joint) const noexcept;
    void update_grappling_mesh(Mesh& mesh, physx::PxJoint* joint);

    void attach_grappling(physx::PxJoint*& grappling);
    void detach_grappling(physx::PxJoint*& grappling);
    void handle_event(const GrapplingEvent& ev, physx::PxJoint*& grappling);

public:
    GrapplingManager(Entity player, Entity camera, 
                     PhysicEngine& physic, Renderer& renderer, 
                     TransformStorage& t, UpdateStorage& u, CameraStorage& c, RigidBodyStorage& r,
                     std::shared_ptr<Materiel> cable_material,
                     PM3D::CDispositifD3D11* dispositif) noexcept;
    ~GrapplingManager();
    void update(FrameDuration dt);
};

#endif