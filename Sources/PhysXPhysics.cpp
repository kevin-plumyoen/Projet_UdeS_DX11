#include "PhysXPhysics.h"

#include <PxPhysics.h>
#include <PxSceneDesc.h>
#include <PxScene.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <thread>
#include <algorithm>

using namespace physx;

PxFilterFlags EnginePhysicFilterShader(PxFilterObjectAttributes, PxFilterData, PxFilterObjectAttributes, PxFilterData, PxPairFlags&, const void*, PxU32);

PhysXPhysics::PhysXPhysics(physx::PxPhysics* physics, bool connected)
: physics{ physics }
, cpu_dispatcher{PxDefaultCpuDispatcherCreate(std::thread::hardware_concurrency() - 1)}
, connected_to_pvd{connected} {
    if (!cpu_dispatcher) {
        physics->release();
        throw FailedToCreateCpuDispatcher{};
    }
}

PhysXPhysics::PhysXPhysics(PhysXPhysics&& physics) noexcept
: physics{ physics.physics}
, cpu_dispatcher{physics.cpu_dispatcher} {
    physics.physics = nullptr;
    physics.cpu_dispatcher = nullptr;
}

PhysXPhysics& PhysXPhysics::operator=(PhysXPhysics&& other) noexcept {
    this->swap(other);
    return *this;
}

PhysXPhysics::~PhysXPhysics() noexcept {
    if(cpu_dispatcher) {
        cpu_dispatcher->release();
    }

    if(physics) {
        physics->release();
    }
}

void PhysXPhysics::swap(PhysXPhysics& other) noexcept {
    using std::swap;

    swap(physics, other.physics);
    swap(cpu_dispatcher, other.cpu_dispatcher);
}

PxScene* PhysXPhysics::make_scene() {
    PxSceneDesc scene_description(physics->getTolerancesScale());
    scene_description.gravity = PxVec3{0.f, -9.8f, 0.f};
    scene_description.cpuDispatcher = cpu_dispatcher;
    scene_description.filterShader = EnginePhysicFilterShader;

    PxScene* scene = physics->createScene(scene_description);
    if (!scene) {
        throw FailedToCreatePhysXScene{};
    }

    return scene;
}

PxPhysics& PhysXPhysics::getPhysics() { 
   return *physics; 
}