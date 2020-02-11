#ifndef DEF_BRIDGE_CONTROLLER_H
#define DEF_BRIDGE_CONTROLLER_H

#include "Entity.h"
#include "Clock.h"
#include "ComponentStorages.h"
#include "Gestionnaires.h"
#include <vector>

class EntityRegistry;

class BridgeController {
    EntityRegistry& entities;
    const GestionnaireTag& tags;
	std::vector<Entity> lights;
public:
	BridgeController(const std::vector<Entity>& lights_, EntityRegistry& entities_, const GestionnaireTag& tags);

	void update(FrameDuration dt);

};

#endif