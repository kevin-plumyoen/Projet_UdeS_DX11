#include "CameraSystem.h"
#include "CameraLen.h"
#include "Logger.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "CameraManager.h"

#include <mutex>

CameraSystem::CameraSystem(CameraManager& manager, CameraStorage& cameras, const TransformStorage& transforms) 
: cameras{cameras}
, transforms{transforms}
, cameras_manager{manager} {

}

void CameraSystem::process(const EntityRegistry& entities, FrameDuration, ProcessState) {
    std::vector<Entity> entities_to_update;
    entities_to_update.reserve(1024);

    // Retrieves all entity with both components
    entities.entities_with(std::back_inserter(entities_to_update), transforms, cameras);

    bool has_changed_active_camera = false;
    bool has_logged_warning = false;

    for (const Entity entity : entities_to_update) {
        const auto transform_it = transforms.find(entity);
        const auto camera_it = cameras.find(entity);

        const Vec3 position = transform_it->second.world.translation;
        const Vec3 direction = transform_it->second.world.forward();
        const Vec3 up = transform_it->second.world.up();

        auto camera_handle_it = entity_camera_map.find(entity);

        // Crée la caméra si elle n'existe pas encore
        if (camera_handle_it == entity_camera_map.end()) {
            // Crée la caméra
            auto camera_handle = cameras_manager.make(camera_it->second.len, position, direction, up);

            camera_it->second.handle_ = camera_handle;

            // Associe la caméra avec cet entité
            camera_handle_it = entity_camera_map.insert(std::make_pair(entity, camera_handle)).first;
        }
        // Met à jour la caméra
        else {
            Camera& camera = cameras_manager[camera_handle_it->second];
            camera.setDirection(direction);
            camera.setUp(up);
            camera.setPosition(position);
        }

        // Active la caméra si elle doit l'être
        if (camera_it->second.active) {
            // Log warning if multiple camera were active this frame
            if (has_changed_active_camera && !has_logged_warning) {
                log_warning("game.ecs.camera_system", "multiple cameras are marked as active");
                has_logged_warning = true;
            }

            // Configure this camera as the active one
            cameras_manager.set_active(camera_handle_it->second);
            camera_it->second.active = false;

            has_changed_active_camera = true;
        }
    }
}

void CameraSystem::cleanup() {
    entity_camera_map.clear();
}