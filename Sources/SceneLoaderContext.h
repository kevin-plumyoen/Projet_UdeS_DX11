#ifndef DEF_SCENE_LOADER_CONTEXT_H
#define DEF_SCENE_LOADER_CONTEXT_H

#include "EntityFactoryAPI.h"
#include "SceneAPI.h"

struct SceneLoaderContext {
    api::EntityFactoryApi entity_factory_api;
    api::SceneApi scene_api;
    Entity camera_entity;

    explicit SceneLoaderContext(EntityFactory* factory) noexcept
    : entity_factory_api{ factory }
    , camera_entity{} {

    }
};

#endif