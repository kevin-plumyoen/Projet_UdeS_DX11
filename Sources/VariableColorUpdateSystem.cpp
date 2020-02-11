#include "VariableColorUpdateSystem.h"

#include <fstream>
std::ofstream file{ "out.txt" };

void VariableColorUpdateSystem::process(const EntityRegistry& entities, FrameDuration dt, ProcessState) {
  hue += color_change_speed()*static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(dt).count());

  if (hue >= DirectX::XM_PI * 2) hue = 0;

  file << hue << std::endl;

  Couleur current_color = hsv2rgb();

  std::vector<Entity> entities_meshes;
  entities_meshes.reserve(32);
  entities.entities_with(std::back_inserter(entities_meshes), variable_colors, static_meshes);

  std::vector<Entity> entities_lights;
  entities_lights.reserve(32);
  entities.entities_with(std::back_inserter(entities_lights), variable_colors, lights);

  for (Entity& mesh_e : entities_meshes) {
    auto& mesh = static_meshes.find(mesh_e)->second;
    mesh.material->emissionColor = current_color;
  }

  for (Entity& light_e : entities_lights) {
    auto& light = lights.find(light_e)->second;
    light.diffuseColor = current_color;
    light.specularColor = current_color;
  }
}