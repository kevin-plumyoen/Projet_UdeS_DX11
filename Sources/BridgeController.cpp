#include "BridgeController.h"
#include "EntityRegistry.h"
#include "EngineEvents.h"
#include "EventBus.h"
#include "Identities.h"

BridgeController::BridgeController(const std::vector<Entity>& lights, EntityRegistry& entities, const GestionnaireTag& tags) 
: entities(entities)
, tags{tags}
, lights(lights)
{
}

void BridgeController::update(FrameDuration)
{
	for (EntityTriggeredEvent current_event : EventBus::instance().events<EntityTriggeredEvent>()) {
        auto tag_it = tags.find(current_event.entity);
        const bool is_triggered_by_player = tag_it != tags.end() ? (tag_it->second & Identities::Player) != 0 : false;
        const bool is_trigger_in_tunnel = current_event.id == Identities::Tunnel;

		if (is_trigger_in_tunnel  && current_event.trigger_event == TriggerEvent::Started && is_triggered_by_player) {
			for (Entity light : lights) {
				entities.enable(light);
			}
		}

		if (is_trigger_in_tunnel && current_event.trigger_event == TriggerEvent::Ended && is_triggered_by_player) {
			for (Entity light : lights) {
				entities.disable(light);
			}
		}
	}

}
