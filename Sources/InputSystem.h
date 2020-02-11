#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "SystemInterface.h"
#include "ComponentStorages.h"
#include "EntityProxyFactory.h"

class InputSystem : public BaseSystem {

  const InputStorage& inputs;
  EntityProxyFactory entity_proxy_factory;
public:
  InputSystem(const InputStorage& inputs, EntityProxyFactory factory) : inputs{ inputs }, entity_proxy_factory{ factory } {}
  void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) override;
};

#endif