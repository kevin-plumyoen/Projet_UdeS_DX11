#include "BaseCameraController.h"

#include "Logger.h"

BaseCameraController::BaseCameraController(Entity camera_entity, TransformStorage& transforms, TransformHierarchy& hierarchy) 
: hierarchy_(hierarchy)
, transforms_{transforms}
, camera_entity_{camera_entity} {
    if (!camera_entity_.valid()) {
        log_warning("base_camera_controller_ctor", "invalid camera entity");
    }
}

Entity BaseCameraController::camera_entity() const noexcept {
    return camera_entity_;
}

TransformStorage& BaseCameraController::transforms() noexcept {
    return transforms_;
}

const TransformStorage& BaseCameraController::transforms() const noexcept {
    return transforms_;
}

TransformComponent& BaseCameraController::camera_transform() noexcept {
    auto transform_it = transforms_.find(camera_entity_);
    assert(transform_it != transforms_.end());

    return transform_it->second;
}

const TransformComponent& BaseCameraController::camera_transform() const noexcept {
    auto transform_it = transforms_.find(camera_entity_);
    assert(transform_it != transforms_.end());

    return transform_it->second;
}