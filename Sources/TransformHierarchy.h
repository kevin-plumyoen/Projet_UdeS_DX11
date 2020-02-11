#ifndef DEF_TRANSFORM_HIERARCHY
#define DEF_TRANSFORM_HIERARCHY

#include "ComponentStorages.h"
#include "EntityRegistry.h"

class TransformHierarchy {
    const EntityRegistry& entities;
    TransformStorage& transforms;

    void update_local_transform(TransformStorage::Iterator transform_it);
    void update_world_transform(TransformStorage::Iterator transform_it);
public:
    static Transform apply_parent_transform(const Transform& parent, const Transform& child) noexcept;
    static Transform apply_parent_inverse_transform(const Transform& parent, const Transform& child) noexcept;

    TransformHierarchy(const EntityRegistry& entities, TransformStorage& transforms) noexcept;

    void update_children_world_transforms(Entity e);
    void update_world_transform(Entity e, const Transform& t);
    void update_world_transform(Entity e, Transform&& t);
    void update_local_transform(Entity e, const Transform& t);
    void update_local_transform(Entity e, Transform&& t);
};

#endif