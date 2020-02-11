#ifndef DEF_PLAYER_CONTROLLER_H
#define DEF_PLAYER_CONTROLLER_H

#include "Entity.h"
#include "Gestionnaires.h"
#include "RadialBlur.h"
#include "ComponentStorages.h"
#include "TransformHierarchy.h"
#include "HUD_Manager.h"


// Gère la transition entre la caméra première personne et la caméra troisième personne 
class PlayerController {

   using RadialBlurPtr = std::shared_ptr<RadialBlur>;

    RadialBlurPtr radial_blur;
    RigidBodyStorage& rbs;
    StaticMeshStorage& static_meshes;
    GestionnaireMateriel& materials;
    EntityRegistry& entities;
    Entity player_entity;
    CHUD_Manager* hud;
    bool is_hidden;

    void update_hud();

    void make_player_invisible();
    void make_player_transparent();
    void make_player_normal();
    void update_material();

public:
   PlayerController(Entity player, CHUD_Manager* hud, GestionnairePostFX& gpFX, GestionnaireMateriel& materials, RigidBodyStorage& rbs, StaticMeshStorage& static_meshes, EntityRegistry& entities);

   void update(FrameDuration dt);
   void late_update(FrameDuration dt);

};

#endif