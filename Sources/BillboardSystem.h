#ifndef DEF_BILLBOARD_SYSTEM_H
#define DEF_BILLBOARD_SYSTEM_H

#include "SystemInterface.h"
#include "ComponentStorages.h"

class CameraManager;

class BillboardSystem : public BaseSystem {
    TransformStorage& transforms;
    const BillboardStorage& billboards;
    const CameraManager& cameras;
public:
    BillboardSystem(TransformStorage& transforms, const BillboardStorage& billboards, const CameraManager& cameras) noexcept;

    void process(const EntityRegistry& entities, FrameDuration dt, ProcessState) override;
};

#endif