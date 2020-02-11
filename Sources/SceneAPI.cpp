#include "SceneAPI.h"
#include "Scene.h"
#include "EntityRegistry.h"

namespace api {

    int add_entity_in_zone(Scene* scene, Entity entity, int zone) {
    scene->getZone(zone).addEntity(entity, scene->getEntities());
	return 0;
	}

}