#ifndef DEF_GAMEPLAY_CAMERA_MANAGER_H
#define DEF_GAMEPLAY_CAMERA_MANAGER_H

#include "Entity.h"
#include "ComponentStorages.h"
#include "TransformHierarchy.h"


// Gère la transition entre la caméra première personne et la caméra troisième personne 
class GameplayCameraManager {
    enum class State : uint8_t {
        FirstPerson,
        ThirdPerson,
        Unknown,
    };

    TransformStorage& transforms;
    CameraStorage& cameras;
    TransformHierarchy& hierarchy;
    RigidBodyStorage& rigid_bodies;
    Entity fp_camera;
    Entity tp_camera;
    Entity player;
    State state;

    void detect_active_camera();

    void transit_to_first_person();
    void transit_to_third_person();

    void lock_rotation();
    void unlock_rotation();

public:
    GameplayCameraManager(Entity fp_camera, Entity tp_camera, Entity player, TransformStorage& transforms, TransformHierarchy& hierarchy, CameraStorage& cameras, RigidBodyStorage& rigid_bodies);

    void update(FrameDuration dt);

    Entity active_camera() const;
};

#endif