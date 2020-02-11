#include "PlayerController.h"
#include "RadialBlur.h"
#include <PxPhysicsAPI.h>
#include <PxRigidDynamic.h>
#include "RigidBodyComponent.h"
#include "PlayerEvent.h"
#include "EventBus.h"

#include <PxPhysicsAPI.h>
#include <PxRigidDynamic.h>


PlayerController::PlayerController(Entity player, CHUD_Manager* hud, GestionnairePostFX& gpFX, GestionnaireMateriel& materials, RigidBodyStorage& rbs, StaticMeshStorage& static_meshes, EntityRegistry& entities)
:rbs{ rbs }
, hud{ hud }
, player_entity{ player }
, static_meshes{static_meshes}
, materials{materials} 
, entities{entities}
, is_hidden{false} {
   radial_blur = std::dynamic_pointer_cast<RadialBlur>(gpFX.at("radial_blur"));
}


void PlayerController::update_hud() {
    auto player_body = rbs.find(player_entity);
    physx::PxVec3 vecteur_vitesse = player_body->second.rigidbody->is<physx::PxRigidBody>()->getLinearVelocity();
    float vitesse = vecteur_vitesse.normalize();
    radial_blur->setVitesse(vitesse);
    hud->update_game_hud_vitesse(vitesse * 3.6f);
}

void PlayerController::update_material() {
    bool update_material = false;
    PlayerRenderingEvent::Event priorised_event = PlayerRenderingEvent::Event::MakePlayerNormal;
    for (PlayerRenderingEvent ev : EventBus::instance().events<PlayerRenderingEvent>()) {
        if (ev.event < priorised_event) {
            priorised_event = ev.event;
        }

        update_material = true;
    }

    if (update_material) {
        switch (priorised_event) {
        case PlayerRenderingEvent::Event::MakePlayerInvisible:
            make_player_invisible();
            break;
        case PlayerRenderingEvent::Event::MakePlayerNormal:
            make_player_normal();
            break;
        case PlayerRenderingEvent::Event::MakePlayerTransparent:
            make_player_transparent();
            break;
        }
    }
}

void PlayerController::make_player_invisible() {
    if(is_hidden) return;
    is_hidden = true;

    auto mesh_it = static_meshes.find(player_entity);

    std::vector<Entity> children;
    children.reserve(2);

    entities.all_children(player_entity, std::back_inserter(children));
    for (Entity child : children) {
        auto static_mesh_it = static_meshes.find(child);

        if (static_mesh_it != static_meshes.end()) {
            static_mesh_it->second.material = materials["transparent"];
        }
    }
}

void PlayerController::make_player_transparent() {
    if(is_hidden) return;

    std::vector<Entity> children;
    children.reserve(2);

    entities.all_children(player_entity, std::back_inserter(children));

    auto core_static_mesh_it = static_meshes.find(children[0]);
    auto frame_static_mesh_it = static_meshes.find(children[1]);

    if (core_static_mesh_it != static_meshes.end()) {
        core_static_mesh_it->second.material = materials["Transparent_Core"];
    }

    if (frame_static_mesh_it != static_meshes.end()) {
        frame_static_mesh_it->second.material = materials["Transparent_Frame"];
    }
}

void PlayerController::make_player_normal() {
    std::vector<Entity> children;
    children.reserve(2);

    entities.all_children(player_entity, std::back_inserter(children));

    auto core_static_mesh_it = static_meshes.find(children[0]);
    auto frame_static_mesh_it = static_meshes.find(children[1]);

    if (core_static_mesh_it != static_meshes.end()) {
        core_static_mesh_it->second.material = materials["Opaque_Core"];
    }

    if (frame_static_mesh_it != static_meshes.end()) {
        frame_static_mesh_it->second.material = materials["Opaque_Frame"];
    }

    is_hidden = false;
}

void PlayerController::update(FrameDuration) {
    update_hud();
}

void PlayerController::late_update(FrameDuration) {
    update_material();
}