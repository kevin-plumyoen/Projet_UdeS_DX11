#ifndef DEF_ECS_SYSTEMS_STATIC_MESH_RENDERING_SYSTEM
#define DEF_ECS_SYSTEMS_STATIC_MESH_RENDERING_SYSTEM

#include "BaseRenderingSystem.h"
#include "ComponentStorages.h"
#include "Scene.h"
#include "Zone.h"

#include "MathHelpers.h"

class Camera;
struct Renderer;

class StaticMeshRenderingSystem : public BaseRenderingSystem {
    StaticMeshStorage& static_meshes;
protected:
    std::vector<Entity> entities_to_draw(const EntityRegistry& entities) const override;
    void draw_entities(Renderer& renderer, std::vector<Entity>&& entities) override;
public:
    StaticMeshRenderingSystem(Renderer& renderer, StaticMeshStorage& static_meshes, const TransformStorage& transforms, Scene& scene);
};

#endif