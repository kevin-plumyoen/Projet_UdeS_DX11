#ifndef DEF_BASE_CAMERA_CONTROLLER_H
#define DEF_BASE_CAMERA_CONTROLLER_H

#include "ComponentStorages.h"
#include "Entity.h"

namespace PM3D {
    class CDIManipulateur;
}

class TransformHierarchy;

class BaseCameraController {
    TransformHierarchy& hierarchy_;
    TransformStorage& transforms_;
    Entity camera_entity_;
protected:
    TransformStorage& transforms() noexcept;
    const TransformStorage& transforms() const noexcept;

    TransformHierarchy& hierarchy() noexcept { return hierarchy_; }
    const TransformHierarchy& hierarchy() const noexcept { return hierarchy_; }
public:
    BaseCameraController(Entity camera_entity, TransformStorage& transforms, TransformHierarchy& hierarchy);
    virtual ~BaseCameraController() = default;

    Entity camera_entity() const noexcept;

    TransformComponent& camera_transform() noexcept;
    const TransformComponent& camera_transform() const noexcept;

    virtual void handle_inputs(const PM3D::CDIManipulateur& input) = 0;
    virtual void update(FrameDuration dt) = 0;
};

#endif