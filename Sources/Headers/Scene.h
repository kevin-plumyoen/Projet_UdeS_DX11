#ifndef DEF_SCENE_H
#define DEF_SCENE_H

#include "Zone.h"
#include "../EntityRegistry.h"

#include <vector>
#include <algorithm>

class Frustrum;

class Scene {
    std::vector<Zone> zones;
	EntityRegistry* entities;
public:

    // ICI elle est en dur, a recoder en utilisant serialize !!!
    Scene(EntityRegistry* entities_);

    void activation(const Frustum& frustum);
    std::vector<Zone> activatedZones() const;

    template<typename OutIt>
    OutIt active_entities(OutIt out) const {
        for (const Zone& zone : zones) {
            if (zone.isActivated()) {
                out = std::copy(zone.getEntities().begin(), zone.getEntities().end(), out);
            }
        }

        return out;
    }

    //void serialize();
    //void unserialize();

    Zone& getZone(int pos) { return zones[pos]; };
	EntityRegistry* getEntities() { return entities; };
};

#endif