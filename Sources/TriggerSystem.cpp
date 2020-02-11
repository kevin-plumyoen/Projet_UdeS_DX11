#include "TriggerSystem.h"
#include "PhysicEngine.h"
#include "EventBus.h"
#include "EngineEvents.h"
#include "Logger.h"

#include <PxRigidActor.h>

TriggerSystem::TriggerSystem(const PhysicEngine& physic, const TriggerStorage& triggers, const RigidBodyStorage& rigidbodies) noexcept 
: physic{physic}
, triggers{triggers}
, rigidbodies{rigidbodies} {

}

static Entity rigidbody_actor_owner(physx::PxRigidActor* actor) noexcept {
    const void* userdata = actor->userData;

    if(userdata) {
        intptr_t entity_bytes = reinterpret_cast<intptr_t>(userdata);
        --entity_bytes;
        Entity e;
        std::memcpy(&e, &entity_bytes, sizeof(Entity));

        return e;
    }

    return Entity{};
}

void TriggerSystem::process(const EntityRegistry& entities, FrameDuration, ProcessState) {
    std::vector<Entity> entities_to_update;
    entities_to_update.reserve(1024);

    // Retrieves all entity with both components
    entities.entities_with(std::back_inserter(entities_to_update), triggers, rigidbodies);

    for (const PhysicSimulationEvents::TriggeredActors& triggered_actor : physic.events().all_triggered()) {
        const Entity trigger_entity = rigidbody_actor_owner(triggered_actor.actors.first);
        const Entity colliding_entity = rigidbody_actor_owner(triggered_actor.actors.second);

        if (trigger_entity.valid()) {
            auto it = std::lower_bound(entities_to_update.begin(), entities_to_update.end(), trigger_entity);
            if (it != entities_to_update.end() && *it == trigger_entity) {
                auto trigger_it = triggers.find(trigger_entity);

                EventBus::instance().transmit(EntityTriggeredEvent(trigger_entity, colliding_entity, triggered_actor.event, trigger_it->second.id, trigger_it->second.userdata));
            }
        }
    }   
}
