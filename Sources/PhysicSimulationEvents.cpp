#include "PhysicSimulationEvents.h"

#include "Logger.h"

using namespace physx;

void PhysicSimulationEvents::onContact(const physx::PxContactPairHeader&, const physx::PxContactPair*, physx::PxU32) {

}

void PhysicSimulationEvents::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {
    for (PxU32 pair_index = 0; pair_index < count; ++pair_index) {
        const PxTriggerPair& pair = pairs[pair_index];
        triggered_actors.emplace_back(std::make_pair(pair.triggerActor, pair.otherActor), pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND ? TriggerEvent::Started : TriggerEvent::Ended);
    }
}

void PhysicSimulationEvents::onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {

}

void PhysicSimulationEvents::onWake(physx::PxActor**, physx::PxU32) {

}

void PhysicSimulationEvents::onSleep(physx::PxActor**, physx::PxU32) {

}

void PhysicSimulationEvents::onAdvance(const physx::PxRigidBody*const*, const physx::PxTransform*, const physx::PxU32) {

}

void PhysicSimulationEvents::clear_triggered() noexcept {
    triggered_actors.clear();
}

const std::vector<PhysicSimulationEvents::TriggeredActors>& PhysicSimulationEvents::all_triggered() const noexcept {
    return triggered_actors;
}