#include "InputSystem.h"
#include "InputCommand.h"
#include "InputController.h"
#include <chrono>

using namespace std::chrono;

void InputSystem::process(const EntityRegistry& entities, FrameDuration frameduration, ProcessState) {
  
  std::vector<Entity> entities_to_update;
  entities_to_update.reserve(64);

  // Retrieves all entity with component
  entities.entities_with(std::back_inserter(entities_to_update), inputs);
  
  for (Entity& e : entities_to_update) {
    EntityProxy ep = entity_proxy_factory.create(e);
    auto controller = inputs.find(e)->second.controller;
    controller->getCommands();
    for (auto command : controller->commands) {
        command->execute(ep, frameduration);
    }
    controller->clearCommands();
  }
  
}
