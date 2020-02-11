#include "TransformHierarchy.h"

TransformHierarchy::TransformHierarchy(const EntityRegistry& entities, TransformStorage& transforms) noexcept
: entities{entities}
, transforms{transforms} {

}

Transform TransformHierarchy::apply_parent_transform(const Transform& parent, const Transform& child) noexcept {
    return child * parent;
}

Transform TransformHierarchy::apply_parent_inverse_transform(const Transform& parent, const Transform& child) noexcept {
    return child * Transform(parent.world_to_local());
}

void TransformHierarchy::update_children_world_transforms(Entity e) {
    std::vector<Entity> to_update_world_transforms;
    to_update_world_transforms.reserve(512);
    entities.all_children(e, std::back_inserter(to_update_world_transforms));

    for (Entity to_update_world_transform : to_update_world_transforms) {
        auto child_transform = transforms.find(to_update_world_transform);
        auto parent_transform = transforms.find(entities.parent_of(to_update_world_transform));

        child_transform->second.world = apply_parent_transform(parent_transform->second.world, child_transform->second.local);
    }
}

void TransformHierarchy::update_world_transform(Entity e, const Transform& t) {
    auto transform_it = transforms.find(e);
    if(transform_it == transforms.end()) return;

    transform_it->second.world = t;

    update_local_transform(transform_it);
    update_children_world_transforms(e);
}

void TransformHierarchy::update_world_transform(Entity e, Transform&& t) {
    auto transform_it = transforms.find(e);
    if(transform_it == transforms.end()) return;

    transform_it->second.world = std::move(t);

    // Update local transform
    update_local_transform(transform_it);
    update_children_world_transforms(e);
}

void TransformHierarchy::update_local_transform(TransformStorage::Iterator transform_it) {
    auto parent_transform_it = transforms.find(entities.parent_of(transform_it->first));
    if (parent_transform_it == transforms.end()) {
        transform_it->second.local = transform_it->second.world;
    }
    else {
        transform_it->second.local = apply_parent_inverse_transform(parent_transform_it->second.world, transform_it->second.world);
    }

}

void TransformHierarchy::update_world_transform(TransformStorage::Iterator transform_it) {
    auto parent_transform_it = transforms.find(entities.parent_of(transform_it->first));
    if (parent_transform_it == transforms.end()) {
        transform_it->second.world = transform_it->second.local;
    }
    else {
        transform_it->second.world = apply_parent_transform(parent_transform_it->second.world, transform_it->second.local);
    }
}

void TransformHierarchy::update_local_transform(Entity e, const Transform& t) {
    auto transform_it = transforms.find(e);
    if(transform_it == transforms.end()) return;

    transform_it->second.local = t;

    update_world_transform(transform_it);
    update_children_world_transforms(e);
}

void TransformHierarchy::update_local_transform(Entity e, Transform&& t) {
    auto transform_it = transforms.find(e);
    if(transform_it == transforms.end()) return;

    transform_it->second.local = std::move(t);

    update_world_transform(transform_it);
    update_children_world_transforms(e);
}