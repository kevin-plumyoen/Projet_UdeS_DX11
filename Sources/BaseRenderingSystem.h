#ifndef DEF_BASE_RENDERING_SYSTEM_H
#define DEF_BASE_RENDERING_SYSTEM_H

#include "SystemInterface.h"
#include "ComponentStorages.h"

class Scene;
struct Renderer;

class BaseRenderingSystem : public BaseSystem {
    const TransformStorage& transforms_;
    Renderer& renderer_;
    const Scene& scene;

    std::vector<Entity> filter_entities_to_draw(std::vector<Entity>&& entities);
protected:
    virtual std::vector<Entity> entities_to_draw(const EntityRegistry& entities) const = 0;
    virtual void draw_entities(Renderer& renderer, std::vector<Entity>&& entities) = 0;

    bool is_culled(PM3D::HitBox hitbox, const TransformComponent& transform) const noexcept;
public:
    BaseRenderingSystem(Renderer& renderer, const TransformStorage& transforms, const Scene& scene);
    void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) override;

    const TransformStorage& transforms() const noexcept { return transforms_; }
};

#endif