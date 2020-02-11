#ifndef DEF_ZONE_H
#define DEF_ZONE_H

#include "MathHelpers.h"
#include "Entity.h"
#include "../EntityRegistry.h"
#include "Frustum.h"
#include "Components.h"

#include <vector>

enum LOD { LOW = 0, HIGH = 1 };

class Zone {
private:
    Vec3 centre = {}, p1 = {}, p2 = {}, p3 = {}, p4 = {}; //Centre et extrémités de la zone
    float demiLongueur = {}, demiLargeur = {};
    float xmin = {}, xmax = {}, ymin = {}, ymax = {};
    std::vector<Entity> entities = {};
    bool activated = false; // true si on souhaite afficher les objets de cette zone
    float distanceLOD = 10.0f; //distance camera-entity limite de changement de LOD
public:

    Zone() = default;
    Zone(Vec3 centre, const std::vector<Entity>& entities, float demiLongueur, float demiLargeur);
    Zone(Vec3 centre, const std::vector<Entity>& entities, float demiDimension)
    : Zone(centre, entities, demiDimension, demiDimension) { }

    void activate(Frustum frustum); // Regarde si une zone est activée
    std::vector<std::pair<Entity, LOD>> lod(Camera &c, const TransformStorage& transforms);

    bool isActivated() const noexcept { return activated; }

    void addEntity(Entity e, EntityRegistry* entity_registry) { 
		entities.push_back(e); 
		for_each(entity_registry->getChilds(e).begin(), entity_registry->getChilds(e).end(), [this, entity_registry](Entity e) {addEntity(e, entity_registry); });
	}

    std::vector<Entity>& getEntities() noexcept { return entities; }
    const std::vector<Entity>& getEntities() const noexcept { return entities; }
};

#endif


