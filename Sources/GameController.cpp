#include "GameController.h"
#include "EventBus.h"
#include "EngineEvents.h"
#include "Identities.h"
#include "HUD_Event.h"

GameController::GameController(const GestionnaireTag& tags)
: tags{tags} {

}

void GameController::check_win_condition() {
    bool has_win = false;

    for (const EntityTriggeredEvent ev : EventBus::instance().events<EntityTriggeredEvent>()) {
        auto tag_it = tags.find(ev.entity);
        if(tag_it == tags.end()) continue;

        // Le joueur a gagné
        if (ev.id == Identities::Goal && tag_it->second == Identities::Player) {
            has_win = true;
        }
    }

    if (has_win) {
        EventBus::instance().transmit(HUDEvent{HUDEvent::Event::AfficherMenuFinDeJeux});
    }
}