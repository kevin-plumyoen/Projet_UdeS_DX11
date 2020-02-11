#include "BillboardRenderingSystem.h"

BillboardRenderingSystem::BillboardRenderingSystem(Renderer& renderer, const TransformStorage& transforms, const BillboardStorage& billboards, const Scene& scene)
: BaseRenderingSystem(renderer, transforms, scene)
, billboards{billboards} {

}

std::vector<Entity> BillboardRenderingSystem::entities_to_draw(const EntityRegistry& entities) const {
    std::vector<Entity> entities_to_draw;

    // Retrieves all entity with both components
    entities.entities_with(std::back_inserter(entities_to_draw), transforms(), billboards);

    return entities_to_draw;
}

void BillboardRenderingSystem::draw_entities(Renderer& renderer, std::vector<Entity>&& entities) {
    for (const Entity& e : entities) {
        auto transform_it = transforms().find(e);
        auto billboard_it = billboards.find(e);

        if (!is_culled(billboard_it->second.billboard->BoundingBox(), transform_it->second)) {
            if(billboard_it->second.material->is_transparent) {
                renderer.push_transparent_command(RenderObject3DCommand(billboard_it->second.billboard.get(), 
                                                                        billboard_it->second.material.get(), 
                                                                        transform_it->second.world.local_to_world()));
            }
            else {
                renderer.push_opaque_command(RenderObject3DCommand(billboard_it->second.billboard.get(), 
                                                                   billboard_it->second.material.get(), 
                                                                   transform_it->second.world.local_to_world()));
            }
        }
    }
}