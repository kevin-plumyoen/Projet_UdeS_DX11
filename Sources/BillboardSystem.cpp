#include "BillboardSystem.h"

#include <DirectXMath.h>

using namespace DirectX;

BillboardSystem::BillboardSystem(TransformStorage& transforms, const BillboardStorage& billboards, const CameraManager& cameras) noexcept
: transforms{transforms}
, billboards{billboards}
, cameras{cameras} {

}

void BillboardSystem::process(const EntityRegistry& entities, FrameDuration /*dt*/, ProcessState) {
    if(!cameras.has_active()) return;

    std::vector<Entity> entities_to_update;
    entities_to_update.reserve(1024);

    // Retrieves all entity with both components
    entities.entities_with(std::back_inserter(entities_to_update), transforms, billboards);
    
    const Camera& active_camera = cameras.active_camera();
    const XMFLOAT3 pos = active_camera.getPosition();
    const XMVECTOR camera_position = XMLoadFloat3(&pos);

    for (const Entity& e : entities_to_update) {
        auto transform_it = transforms.find(e);
        auto billboard_it = billboards.find(e);

        const XMVECTOR billboard_position = XMLoadFloat3(&transform_it->second.world.translation);
        const XMVECTOR camera_direction = camera_position - billboard_position;

        switch (billboard_it->second.orientation) {
        case BillboardLinearOrientation::OnlyZ: {
            XMVECTOR billboard_direction_ = XMVectorSetZ(camera_direction, 0.f);
            XMVECTOR billboard_target_ = billboard_position + billboard_direction_;

            XMFLOAT3 billboard_target;
            XMStoreFloat3(&billboard_target, billboard_target_);

            transform_it->second.local.look_at(billboard_target);
        }
            break;
        case BillboardLinearOrientation::LookAt: {
            transform_it->second.local.look_at(active_camera.getPosition());
        }
            break;
        }
    }
}