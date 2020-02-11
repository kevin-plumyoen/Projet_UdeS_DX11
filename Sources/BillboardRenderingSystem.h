#ifndef DEF_BILLBOARD_RENDERING_H
#define DEF_BILLBOARD_RENDERING_H

#include "ComponentStorages.h"
#include "BaseRenderingSystem.h"
#include "Renderer.h"

class BillboardRenderingSystem : public BaseRenderingSystem {
    const BillboardStorage& billboards;

    std::vector<Entity> entities_to_draw(const EntityRegistry& entities) const override;
    void draw_entities(Renderer& renderer, std::vector<Entity>&& entities) override;

public:
    BillboardRenderingSystem(Renderer& renderer, const TransformStorage& transforms, const BillboardStorage& billboards, const Scene& scene);
};

#endif