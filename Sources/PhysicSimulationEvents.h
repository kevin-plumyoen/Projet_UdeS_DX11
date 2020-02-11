#ifndef DEF_PHYSIC_SIMULATION_EVENTS_H
#define DEF_PHYSIC_SIMULATION_EVENTS_H

#include "TriggerEvent.h"

#include <PxSimulationEventCallback.h>
#include <vector>
#include <utility>

class PhysicSimulationEvents : public physx::PxSimulationEventCallback {
public:
    struct TriggeredActors {
        std::pair<physx::PxRigidActor*, physx::PxRigidActor*> actors;
        TriggerEvent event;

        constexpr TriggeredActors(std::pair<physx::PxRigidActor*, physx::PxRigidActor*> actors, TriggerEvent ev) noexcept
        : actors{ actors }
        , event {ev} {

        }
    };
private:
    std::vector<TriggeredActors> triggered_actors;
public:
    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
	void onTrigger(physx::PxTriggerPair*, physx::PxU32) override;
	void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override;
	void onWake(physx::PxActor** , physx::PxU32 ) override;
	void onSleep(physx::PxActor** , physx::PxU32 ) override;
	void onAdvance(const physx::PxRigidBody*const*, const physx::PxTransform*, const physx::PxU32) override;

    void clear_triggered() noexcept;
    const std::vector<TriggeredActors>& all_triggered() const noexcept;
};

#endif