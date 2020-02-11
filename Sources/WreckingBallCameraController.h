#ifndef DEF_WRECKING_BALL_CAMERA_CONTROLLER_H
#define DEF_WRECKING_BALL_CAMERA_CONTROLLER_H

#include "RayCastedThirdPersonCameraController.h"
#include "MathHelpers.h"

class WreckingBallCameraController : public RayCastedThirdPersonCameraController {
    StaticMeshStorage& meshes;
    Vec2 mouse_movement;

    void find_optimal_position();
    void handle_player_view_obstruction();
public:
    WreckingBallCameraController(Entity camera, Entity target, TransformStorage& transforms, TransformHierarchy& hierarchy, PhysicEngine& physic) noexcept;
    void handle_inputs(const PM3D::CDIManipulateur& input) override;
    void update(FrameDuration dt) override;
};

#endif