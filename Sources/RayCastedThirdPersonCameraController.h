#ifndef DEF_RAYCASTED_THIRD_PERSON_CAMERA_CONTROLLER_H
#define DEF_RAYCASTED_THIRD_PERSON_CAMERA_CONTROLLER_H

#include "ThirdPersonCameraController.h"

#include <optional>

class PhysicEngine;

class RayCastedThirdPersonCameraController : public ThirdPersonCameraController {
    PhysicEngine& physic;
protected:
    std::optional<Vec3> position_adjustment_in_environment(Vec3 new_position, Vec3 new_direction) const noexcept;

    bool is_view_obstructed_by_target() const noexcept;
public:
    RayCastedThirdPersonCameraController(Entity camera_entity, Entity target_entity, TransformStorage& transforms, TransformHierarchy& hierarchy, PhysicEngine& physic, float distance);

};

#endif