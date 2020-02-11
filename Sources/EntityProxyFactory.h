#ifndef ENTITYPROXYFACTORY_H
#define ENTITYPROXYFACTORY_H

#include "EntityProxy.h"
#include "Components.h"
#include <map>

struct EntityProxyFactory {
  EntityProxyFactory(Components& components) : components{ components }{}

  std::map<Entity, EntityProxy> entityToProxy;
  Components& components;

  EntityProxy& create(Entity& e) {
    if (entityToProxy.find(e) == entityToProxy.end()) {
      EntityProxy ep;

      ep.input = (components.inputs.is_attached(e)) ? &(components.inputs.find(e)->second) : nullptr;
      ep.light = (components.lights.is_attached(e)) ? &(components.lights.find(e)->second) : nullptr;
      ep.camera = (components.cameras.is_attached(e)) ? &(components.cameras.find(e)->second) : nullptr;
      ep.transform = (components.transforms.is_attached(e)) ? &(components.transforms.find(e)->second) : nullptr;
      ep.rigid_body = (components.rigidbodies.is_attached(e)) ? &(components.rigidbodies.find(e)->second) : nullptr;
      ep.static_mesh = (components.static_meshes.is_attached(e)) ? &(components.static_meshes.find(e)->second) : nullptr;

      entityToProxy[e] = ep;
    }
    
    return entityToProxy[e];
  }

};

#endif