#include "BaseRenderingSystem.h"
#include "Scene.h"
#include "Renderer.h"

#ifndef NEDITOR
#include "CheatDatabase.h"
#endif

BaseRenderingSystem::BaseRenderingSystem(Renderer& renderer, const TransformStorage& transforms, const Scene& scene) 
: transforms_{transforms}
, renderer_{renderer}
, scene{scene} {

}

std::vector<Entity> BaseRenderingSystem::filter_entities_to_draw(std::vector<Entity>&& entities) {
    std::vector<Entity> entities_actif;
    entities_actif.reserve(256);
    scene.active_entities(std::back_inserter(entities_actif));
    std::sort(entities_actif.begin(), entities_actif.end());
    
    std::vector<Entity> to_draw;
    std::set_intersection(entities.begin(), entities.end(), 
                          entities_actif.begin(), entities_actif.end(), 
                          std::back_inserter(to_draw));
    return to_draw;
}

bool BaseRenderingSystem::is_culled(PM3D::HitBox hitbox, const TransformComponent& transform) const noexcept {
#ifndef NEDITOR
    if(CheatDatabase::get().disable_culling) return false;
#endif
    hitbox.transform(transform.world);
    return !renderer_.cameras().active_camera().frustum().contains(hitbox);
}

void BaseRenderingSystem::process(const EntityRegistry& entities, FrameDuration, ProcessState) {
    draw_entities(renderer_, filter_entities_to_draw(entities_to_draw(entities)));
}