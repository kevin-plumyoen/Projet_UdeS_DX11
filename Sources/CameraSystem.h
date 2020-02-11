#ifndef DEF_CAMERA_SYSTEM_H
#define DEF_CAMERA_SYSTEM_H

#include "SystemInterface.h"
#include "ComponentStorages.h"

#include <map>

class CameraManager;

class CameraSystem : public BaseSystem {
    CameraStorage& cameras;
    const TransformStorage& transforms;
    CameraManager& cameras_manager;

    std::map<Entity, std::size_t> entity_camera_map;
public:
    CameraSystem(CameraManager& manager, CameraStorage& cameras, const TransformStorage& transforms);
    void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) override;

    void cleanup() override;
};

#endif