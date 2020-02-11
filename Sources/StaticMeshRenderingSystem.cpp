#include "StaticMeshRenderingSystem.h"
#include "Camera.h"
#include "DrawContext.h"
#include "Shader.h"
#include "Renderer.h"
#include "Scene.h"
#include "Hitbox.h"
#include "StaticMeshComponent.h"

using namespace DirectX;

StaticMeshRenderingSystem::StaticMeshRenderingSystem(Renderer& renderer, StaticMeshStorage& static_meshes, const TransformStorage& transforms, Scene& scene)
: BaseRenderingSystem(renderer, transforms, scene)
, static_meshes{static_meshes} {

}

std::vector<Entity> StaticMeshRenderingSystem::entities_to_draw(const EntityRegistry& entities) const {
    std::vector<Entity> entities_with_static_mesh;
    entities.entities_with(std::back_inserter(entities_with_static_mesh), transforms(), static_meshes);

    return entities_with_static_mesh;
}

void StaticMeshRenderingSystem::draw_entities(Renderer& renderer, std::vector<Entity>&& entities) {
    for (const Entity& e : entities) {
        auto transform_it = transforms().find(e);
        auto static_mesh_it = static_meshes.find(e);
        
        if (!is_culled(static_mesh_it->second.mesh->BoundingBox(), transform_it->second)) {
            if (static_mesh_it->second.material->is_transparent) {
                renderer.push_transparent_command(RenderObject3DCommand(static_mesh_it->second.mesh.get(),
                                                                        static_mesh_it->second.material.get(),
                                                                        transform_it->second.world.local_to_world()));
            }
            else {
                renderer.push_opaque_command(RenderObject3DCommand(static_mesh_it->second.mesh.get(),
                                                                   static_mesh_it->second.material.get(),
                                                                   transform_it->second.world.local_to_world()));
            }
        }
    }
}