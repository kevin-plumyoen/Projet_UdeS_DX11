#include "PhysicEngine.h"
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <thread>
#include <iostream>
#include <cassert>
#include <DirectXMath.h>

#include "Logger.h"

using namespace physx;
using namespace std;

PxFilterFlags EnginePhysicFilterShader(PxFilterObjectAttributes attributes0, PxFilterData /*filterData0*/,
                                       PxFilterObjectAttributes attributes1, PxFilterData /*filterData1*/,
                                       PxPairFlags& pairFlags, const void* /*constantBlock*/, PxU32 /*constantBlockSize*/) { 
    //const PhysicEngine* engine = reinterpret_cast<const PhysicEngine*>(constantBlock);
    
    if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlags();
	}

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	return PxFilterFlags();
}

RaycastHit::RaycastHit(PxRigidActor* actor, const Vec3& position, const Vec3& normal, float distance)
: actor_{actor}
, position_{position}
, normal_{normal}
, distance_{distance} {

}

PhysicEngine::PhysicEngine()
: allocator{ std::make_unique<PxDefaultAllocator>() }
, error{ std::make_unique<PxDefaultErrorCallback>() }
, foundation{ *allocator, *error }
, physics{ foundation.make_physics() }
, scene{ physics.make_scene() }
, simulation_events{}
, duration_acc{} {
    scene->setSimulationEventCallback(&simulation_events);
    scene->setFilterShaderData(this, sizeof(PhysicEngine*));
    PxPvdSceneClient* pvd_client = scene->getScenePvdClient();
    if (pvd_client) {
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        //pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
    }
}

PhysicEngine::~PhysicEngine() {
    scene->release();
}

void PhysicEngine::simulate(FrameDuration dt) {
    const FrameDuration physic_update_dt(std::chrono::microseconds(16670));
    duration_acc += dt;

    while (duration_acc >= physic_update_dt) {
        scene->simulate(1.f / 60.f);
        scene->fetchResults(true);
        duration_acc -= physic_update_dt;
    }
}

PhysicSimulationEvents& PhysicEngine::events() noexcept {
    return simulation_events;
}

const PhysicSimulationEvents& PhysicEngine::events() const noexcept {
    return simulation_events;
}

class CreateHeightFieldFail {};

PxRigidStatic* PhysicEngine::createHeightField(const vector<float>& heightmap, float scaleWidth, float scaleDepth, uint32_t width, uint32_t depth)
{
    const PxReal heightScale = 0.01f;

    PxReal hfScaleWidth = PxReal(scaleWidth);
    PxReal hfScaleDepth = PxReal(scaleDepth);
    PxU32 hfWidth = PxU32(width);
    PxU32 hfDepth = PxU32(depth);


    PxU32 hfNumVerts = hfWidth * hfDepth;

    PxHeightFieldSample* samples = (PxHeightFieldSample*)malloc(sizeof(PxHeightFieldSample)*hfNumVerts);
    memset(samples, 0, hfNumVerts * sizeof(PxHeightFieldSample));

    for (PxU32 x = 0; x < hfWidth; x++)
        for (PxU32 y = 0; y < hfDepth; y++)
        {
            PxI32 h = PxI32(heightmap[y + x * hfWidth] / heightScale);
            PX_ASSERT(h <= 0xffff);
            samples[x + y * hfWidth].height = (PxI16)(h);
            samples[x + y * hfWidth].setTessFlag();
            samples[x + y * hfWidth].materialIndex0 = 1;
            samples[x + y * hfWidth].materialIndex1 = 1;
        }

    PxHeightFieldDesc hfDesc;
    hfDesc.format = PxHeightFieldFormat::eS16_TM;
    hfDesc.nbColumns = hfWidth;
    hfDesc.nbRows = hfDepth;
    hfDesc.samples.data = samples;
    hfDesc.samples.stride = sizeof(PxHeightFieldSample);

    PxTolerancesScale scale;
    PxCookingParams params(scale);
    params.meshWeldTolerance = 0.001f;
    params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
    params.buildGPUData = true; //Enable GRB data being produced in cooking.
    PxCooking *mCooking = PxCreateCooking(PX_PHYSICS_VERSION, foundation.getFoundation(), params);
    if (!mCooking)
       throw CreateHeightFieldFail{};

    PxHeightField* heightField = mCooking->createHeightField(hfDesc, physics.getPhysics().getPhysicsInsertionCallback());
    if (!heightField)
       throw CreateHeightFieldFail{};

    PxTransform pose = PxTransform(PxIdentity);

    PxRigidStatic* hfActor = physics.getPhysics().createRigidStatic(pose);
    if (!hfActor)
       throw CreateHeightFieldFail{};

    PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, hfScaleWidth, hfScaleDepth);
    PxMaterial* defaultMaterial = physics.getPhysics().createMaterial(0.5f, 0.5f, 0.1f);
    if (!defaultMaterial)
       throw CreateHeightFieldFail{};
    PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *defaultMaterial);
    if (!hfShape)
       throw CreateHeightFieldFail{};

    scene->addActor(*hfActor);

    free(samples);

    return hfActor;
}

static PxTransform to_transform(const Transform& t) noexcept {
    PxTransform transform;
    transform.p = PhysicEngine::positionMotor2PhysX(t.translation);
    transform.q = PhysicEngine::rotationMotor2PhysX(t.rotation);

    return transform;
}

static PxTransform to_transform(const Vec3& location, const Quaternion& rotation) noexcept {
    PxTransform transform;
    transform.p = PhysicEngine::positionMotor2PhysX(location);
    transform.q = PhysicEngine::rotationMotor2PhysX(rotation);

    return transform;
}

static PxTransform to_transform(const Vec3& location, const Vec3& rotation) noexcept {
    const Quaternion new_rotation = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMFLOAT4 new_rotation_elements;
    DirectX::XMStoreFloat4(&new_rotation_elements, new_rotation);

    return to_transform(location, new_rotation);
}

PxShape* PhysicEngine::create_sphere_shape(float radius, const PhysicMaterial& material) {
    PxMaterial* px_material = physics.getPhysics().createMaterial(material.static_friction, material.dynamic_friction, material.restitution);

    PxShape* px_shape = physics.getPhysics().createShape(PxSphereGeometry(radius), *px_material);
    px_material->release();

    return px_shape;
}

PxShape* PhysicEngine::create_box_shape(Vec3 scaling, const PhysicMaterial& material) {
	PxMaterial* px_material = physics.getPhysics().createMaterial(material.static_friction, material.dynamic_friction, material.restitution);

	PxShape* px_shape = physics.getPhysics().createShape(PxBoxGeometry(scaling.x, scaling.z, scaling.y), *px_material);
	px_material->release();

	return px_shape;
}

PxRigidStatic* PhysicEngine::createStaticSphere(float radius, const PhysicMaterial& material, Vec3 location, Vec3 rotation) {
    assert(radius > 0.f);

    // Crée un matériel à chaque fois
    PxShape* px_shape = create_sphere_shape(radius, material);

    PxRigidStatic* new_body = physics.getPhysics().createRigidStatic(to_transform(location, rotation));
    new_body->attachShape(*px_shape);
    px_shape->release();

    scene->addActor(*new_body);

    return new_body;
}

PxRigidStatic* PhysicEngine::createStaticCube(const PhysicMaterial& material, Vec3 location, Vec3 rotation, Vec3 scaling) {
	assert(scaling.x > 0.f && scaling.y > 0.f && scaling.z > 0.f);

	// Crée un matériel à chaque fois
	PxShape* px_shape = create_box_shape(scaling, material);

	PxRigidStatic* new_body = physics.getPhysics().createRigidStatic(to_transform(location, rotation));
	new_body->attachShape(*px_shape);
	px_shape->release();

	scene->addActor(*new_body);

	return new_body;
}

PxRigidStatic* PhysicEngine::createStaticCube(const PhysicMaterial& material, const Transform& t) {
    assert(t.scaling.x > 0.f && t.scaling.y > 0.f && t.scaling.z > 0.f);

    PxShape* px_shape = create_box_shape(t.scaling, material);

	PxRigidStatic* new_body = physics.getPhysics().createRigidStatic(to_transform(t));
	new_body->attachShape(*px_shape);
	px_shape->release();

	scene->addActor(*new_body);

	return new_body;
}

PxRigidDynamic* PhysicEngine::createDynamicSphere(float radius, const PhysicMaterial& material, Vec3 location, Vec3 rotation) {
    assert(radius > 0.f);
    if(material.mass <= 0.f) throw MissingMass{};

    // Crée un matériel à chaque fois
    PxShape* px_shape = create_sphere_shape(radius, material);

    PxRigidDynamic* new_body = physics.getPhysics().createRigidDynamic(to_transform(location, rotation));
    new_body->attachShape(*px_shape);
    PxRigidBodyExt::updateMassAndInertia(*new_body, material.mass);
    px_shape->release();

    scene->addActor(*new_body);

    return new_body;
}

PxRigidDynamic* PhysicEngine::createDynamicCube(const PhysicMaterial& material, Vec3 location, Vec3 rotation, Vec3 scaling) {
    assert(scaling.x > 0.f && scaling.y > 0.f && scaling.z > 0.f);
    if(material.mass <= 0.f) throw MissingMass{};

    // Crée un matériel à chaque fois
    PxShape* px_shape = create_box_shape(scaling, material);

    PxRigidDynamic* new_body = physics.getPhysics().createRigidDynamic(to_transform(location, rotation));
    new_body->attachShape(*px_shape);
    PxRigidBodyExt::updateMassAndInertia(*new_body, material.mass);
    px_shape->release();

    scene->addActor(*new_body);

    return new_body;
}

PxRigidStatic* PhysicEngine::createStaticTriggerSphere(float radius, const PhysicMaterial& material, Vec3 location, Vec3 rotation) {
    assert(radius > 0.f);

    PxMaterial* px_material = physics.getPhysics().createMaterial(material.static_friction, material.dynamic_friction, material.restitution);

    PxShape* shape = physics.getPhysics().createShape(PxSphereGeometry(radius), *px_material);
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

    PxRigidStatic* trigger = physics.getPhysics().createRigidStatic(to_transform(location, rotation));
    trigger->attachShape(*shape);
    shape->release();

    scene->addActor(*trigger);

    return trigger;
}

PxRigidStatic* PhysicEngine::createStaticTriggerBox(Vec3 half_extend, const PhysicMaterial& material, Vec3 location, Vec3 rotation) {
    assert(half_extend.x > 0.f);
    assert(half_extend.y > 0.f);
    assert(half_extend.z > 0.f);

    PxMaterial* px_material = physics.getPhysics().createMaterial(material.static_friction, material.dynamic_friction, material.restitution);

    PxShape* shape = physics.getPhysics().createShape(PxBoxGeometry(half_extend.x, half_extend.z, half_extend.y), *px_material);
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

    PxRigidStatic* trigger = physics.getPhysics().createRigidStatic(to_transform(location, rotation));
    trigger->attachShape(*shape);
    shape->release();

    scene->addActor(*trigger);

    return trigger;
}

void PhysicEngine::destroyActor(PxRigidActor* actor) {
    scene->removeActor(*actor);
    actor->release();
}

PxJoint* PhysicEngine::createDistanceJoint(PxRigidActor* a, Vec3 location_a, PxRigidActor* b, Vec3 location_b, float max_distance) {
    PxDistanceJoint* px_joint = PxDistanceJointCreate(physics.getPhysics(), 
                                                      a, PxTransform(positionMotor2PhysX(location_a)),
                                                      b, PxTransform(positionMotor2PhysX(location_b)));
    px_joint->setMaxDistance(max_distance);

    return px_joint;
}

void PhysicEngine::destroyJoint(PxJoint* joint) {
    joint->release();
}

std::optional<RaycastHit> PhysicEngine::raycast(Vec3 origin, Vec3 direction, float max_distance) {
    PxRaycastBuffer hit;

    const PxVec3 px_origin = positionMotor2PhysX(origin);
    const PxVec3 px_direction = positionMotor2PhysX(direction);

    if (scene->raycast(px_origin, px_direction, max_distance, hit)) {
        return RaycastHit(hit.block.actor, 
                          positionPhysX2Motor(hit.block.position), 
                          positionPhysX2Motor(hit.block.normal), 
                          hit.block.distance);
    }

    return {};
}

std::optional<RaycastHit> PhysicEngine::raycast(Vec3 origin, Vec3 direction, float max_distance, uint32_t collision_mask) {
    PxRaycastBuffer hit;

    const PxVec3 px_origin = positionMotor2PhysX(origin);
    const PxVec3 px_direction = positionMotor2PhysX(direction);

    PxQueryFilterData filter_data;
    filter_data.data.word0 = collision_mask;

    if (scene->raycast(px_origin, px_direction, max_distance, hit, PxHitFlag::eDEFAULT, filter_data)) {
        return RaycastHit(hit.block.actor, 
                          positionPhysX2Motor(hit.block.position), 
                          positionPhysX2Motor(hit.block.normal), 
                          hit.block.distance);
    }

    return {};
}

bool PhysicEngine::is_grounded(PxRigidActor* actor) const noexcept {
    class GroundQueryFilter : public PxQueryFilterCallback {
    public:
        PxQueryHitType::Enum preFilter(const PxFilterData&, const PxShape*, const PxRigidActor*, PxHitFlags&) override {
            return PxQueryHitType::eBLOCK;
        }

        PxQueryHitType::Enum postFilter(const PxFilterData& , const PxQueryHit& hit) override {
            if (static_cast<const PxRaycastHit&>(hit).hadInitialOverlap()) {
                return PxQueryHitType::eNONE;
            }

            return PxQueryHitType::eBLOCK;
        }
    };

    const PxVec3 origin = actor->getGlobalPose().p;
    const float extent = actor->getWorldBounds().getExtents().y + 0.05f;
    const PxVec3 direction(0.f, -1.f, 0.f);

    GroundQueryFilter ground_filter;

    PxQueryFilterData filter_data = PxQueryFilterData();
    filter_data.flags |= PxQueryFlag::ePOSTFILTER;

    PxRaycastBuffer hit;
    if (scene->raycast(origin, direction, extent, hit, PxHitFlag::eDEFAULT, filter_data, &ground_filter)) {
        return true;
    }

    return false;
}

bool PhysicEngine::is_connected_to_pvd() const noexcept {
    return physics.is_connected();
}

PxVec3 PhysicEngine::positionMotor2PhysX(Vec3 vec) {
   return PxVec3(vec.x, vec.z, vec.y);
}

Vec3 PhysicEngine::positionPhysX2Motor(PxVec3 vec) {
   return Vec3(vec.x, vec.z, vec.y);
}

PxQuat PhysicEngine::rotationMotor2PhysX(Quaternion rot) {
    DirectX::XMFLOAT4 quat;
    DirectX::XMStoreFloat4(&quat, rot);
    return PxQuat(  -quat.x , -quat.z, -quat.y, quat.w);
}

Quaternion PhysicEngine::rotationPhysX2Motor(PxQuat rot) {
    return Quaternion{ -rot.x, -rot.z, -rot.y, rot.w };
}