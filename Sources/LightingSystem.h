#ifndef LIGHTINGSYSTEM_H
#define LIGHTINGSYSTEM_H

#include "SystemInterface.h"
#include "ComponentStorages.h"

class LightManager;

class LightingSystem : public BaseSystem {
    LightStorage& lights;
    const TransformStorage& transforms;

    LightManager& light_manager;

public:
    LightingSystem(const TransformStorage& transforms, LightStorage& lights, LightManager& light_manager);

    void process(const EntityRegistry& entities, FrameDuration, ProcessState) override;

};

#endif