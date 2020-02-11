#include "HierarchicalTransformSystem.h"
#include "TransformHierarchy.h"

HierarchicalTransformSystem::HierarchicalTransformSystem(TransformStorage& transforms, TransformHierarchy& hierarchy)
: BaseTransformModifierSystem{transforms, hierarchy} {

}

std::size_t parent_count(const EntityRegistry& entities, Entity e) {
    if (entities.is_child(e)) {
        return 1 + parent_count(entities, entities.parent_of(e));
    }

    return 0;
}

void HierarchicalTransformSystem::process(const EntityRegistry& entities, FrameDuration, ProcessState) {
    // Update transforms hiearchically
    std::vector<Entity> entities_to_update;
    entities_to_update.reserve(1024);

    entities.entities_with(std::back_inserter(entities_to_update), transforms);
    
    // On trie les entités 
    std::sort(entities_to_update.begin(), entities_to_update.end(), [&entities](const Entity& a, const Entity& b) {
        return parent_count(entities, a) < parent_count(entities, b);
    });

    for (Entity entity_to_update : entities_to_update) {
        auto transform_it = transforms.find(entity_to_update);
        auto parent_transform_it = transforms.find(entities.parent_of(entity_to_update));

        // S'il n'a pas de parent
        if (parent_transform_it == transforms.end()) {
            transform_it->second.world = transform_it->second.local;
        }
        // Sinon notre world = world du parent * notre local
        else {
            transform_it->second.world = TransformHierarchy::apply_parent_transform(parent_transform_it->second.world, transform_it->second.local);
        }
    }
}