#ifndef DEF_PHYSIC_ENGINE_H
#define DEF_PHYSIC_ENGINE_H

#include "Clock.h"
#include "PhysXFoundation.h"
#include "PhysXPhysics.h"
#include "MathHelpers.h"
#include "PhysicMaterial.h"
#include "PhysicSimulationEvents.h"
#include "Transform.h"

#include <foundation/PxVec3.h>
#include <foundation/PxQuat.h>

#include <memory>
#include <vector>
#include <optional>

namespace physx {
    class PxAllocatorCallback;
    class PxErrorCallback;
    class PxScene;
    class PxRigidActor;
    class PxRigidStatic;
    class PxRigidDynamic;
    class PxShape;
    class PxJoint;
}
using namespace physx;

class RaycastHit {
    PxRigidActor* actor_;
    Vec3 position_;
    Vec3 normal_;
    float distance_;

public:
    RaycastHit(PxRigidActor* actor, const Vec3& position, const Vec3& normal, float distance);

    PxRigidActor* actor() const noexcept { return actor_; }
    const Vec3& position() const noexcept { return position_; }
    const Vec3& normal() const noexcept { return normal_; }
    float distance() const noexcept { return distance_; }
};


class PhysicEngine {
public:
    struct MissingMass{};
private:
    std::unique_ptr<physx::PxAllocatorCallback> allocator;
    std::unique_ptr<physx::PxErrorCallback> error;
    PhysXFoundation foundation;
    PhysXPhysics physics;
    physx::PxScene* scene;
    PhysicSimulationEvents simulation_events;
    FrameDuration duration_acc;
    
    PxShape* create_sphere_shape(float radius, const PhysicMaterial& material);
	PxShape* create_box_shape(Vec3 scaling, const PhysicMaterial& material);
public:
    PhysicEngine();
    ~PhysicEngine();

    void simulate(FrameDuration dt);

    PhysicSimulationEvents& events() noexcept;
    const PhysicSimulationEvents& events() const noexcept;

    // utiliser foundation && physics && scene
    PxRigidStatic* createHeightField(const std::vector<float>& heightmap, float hfScaleWidth, float hfScaleDepth, uint32_t hfWidth, uint32_t hfDepth);
	PxRigidStatic* createStaticSphere(float radius, const PhysicMaterial& material, Vec3 location, Vec3 rotation);
	PxRigidStatic* createStaticCube(const PhysicMaterial& material, Vec3 location, Vec3 rotation, Vec3 scaling);
    PxRigidStatic* createStaticCube(const PhysicMaterial& material, const Transform& t);
    PxRigidDynamic* createDynamicSphere(float radius, const PhysicMaterial& material, Vec3 location, Vec3 rotation);
    PxRigidDynamic* createDynamicCube(const PhysicMaterial& material, Vec3 location, Vec3 rotation, Vec3 scaling);

    // Trigger shapes
    PxRigidStatic* createStaticTriggerSphere(float radius, const PhysicMaterial& material, Vec3 location, Vec3 rotation);
    PxRigidStatic* createStaticTriggerBox(Vec3 half_extend, const PhysicMaterial& material, Vec3 location, Vec3 rotation);

    void destroyActor(PxRigidActor* actor);

    PxJoint* createDistanceJoint(PxRigidActor* a, Vec3 location_a, PxRigidActor* b, Vec3 location_b, float max_distance);
    void destroyJoint(PxJoint* joint);

    std::optional<RaycastHit> raycast(Vec3 origin, Vec3 direction, float max_distance);
    std::optional<RaycastHit> raycast(Vec3 origin, Vec3 direction, float max_distance, uint32_t collision_mask);

    // Verify if an actor is grounded
    bool is_grounded(PxRigidActor* actor) const noexcept;

    bool is_connected_to_pvd() const noexcept;

    static PxVec3 positionMotor2PhysX(Vec3 vec);
    static Vec3 positionPhysX2Motor(PxVec3 vec);
    static PxQuat rotationMotor2PhysX(Quaternion rot);
    static Quaternion rotationPhysX2Motor(PxQuat rot);

};

#endif