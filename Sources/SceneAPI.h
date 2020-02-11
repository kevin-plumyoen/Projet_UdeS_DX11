#ifndef DEF_SCENE_API_H
#define DEF_SCENE_API_H

#include "Entity.h"
#include "EntityRegistry.h"
class Scene;

namespace api {

int add_entity_in_zone(Scene* scene, Entity entity, int zone);

struct SceneApi {
    using add_entity_in_zone_fn = int(*)(Scene*, Entity, int);

    add_entity_in_zone_fn add_entity_in_zone;
    Scene* scene;
};

}

#endif