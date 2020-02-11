#include "LightingSystem.h"
#include "Shader.h"
#include "LightManager.h"
#include "Logger.h"

#include <utility>

LightingSystem::LightingSystem(const TransformStorage& transforms, LightStorage& lights, LightManager& light_manager)
: lights{ lights }
, transforms{ transforms }
, light_manager{ light_manager } {

}

void LightingSystem::process(const EntityRegistry& entities, FrameDuration, ProcessState) {
    std::vector<Entity> entities_lights;
    entities_lights.reserve(32);
    entities.entities_with(std::back_inserter(entities_lights), lights);

    light_manager.clearLights();

    for (Entity& l : entities_lights) {
        auto light = lights.find(l);
        auto transform_light_it = transforms.find(l);

        if (light->second.is_type<PointLight>()) {
            if (transform_light_it == transforms.end()) {
                log_warning("lighing_system.process", "missing transform on point light");
                continue;
            }

            const Vec3 transformed_offset = transform_light_it->second.world.transform_point(light->second.offset);

            Shader::ShadersPointLight buffer = std::get<Shader::ShadersPointLight>(light->second.getParams());
            buffer.pos = XMLoadFloat3(&transformed_offset); //XMLoadFloat3(&(transform_light_it->second.world.translation)) + XMLoadFloat3(&(light->second.offset));
            
            light_manager.addPointLight(buffer);
        };

        if (light->second.is_type<DirectionalLight>()) {
            Shader::ShadersDirLight buffer = std::get<Shader::ShadersDirLight>(light->second.getParams());

            light_manager.addDirLight(buffer);
        };

        if (light->second.is_type<SpotLight>()) {
            if (transform_light_it == transforms.end()) {
                log_warning("lighing_system.process", "missing transform on spot light");
                continue;
            }

            const Vec3 transformed_offset = transform_light_it->second.world.transform_point(light->second.offset);
            const Vec3 transformed_forward = transform_light_it->second.world.forward();

            Shader::ShadersSpotLight buffer = std::get<Shader::ShadersSpotLight>(light->second.getParams());
            buffer.dir = XMLoadFloat3(&transformed_forward);
            buffer.pos = XMLoadFloat3(&transformed_offset);

            light_manager.addSpotLight(buffer);
        };
    }
}