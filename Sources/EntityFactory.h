#ifndef DEF_ECS_ENTITY_FACTORY_H
#define DEF_ECS_ENTITY_FACTORY_H

#include "Components.h"
#include "Gestionnaires.h"
#include "RequiredSystems.h"
#include "HUD_Manager.h"

#include <optional>
#include <vector>

class EntityRegistry;
class PhysicEngine;

namespace physx {
    class PxRigidStatic;
    class PxRigidDynamic;
}

class EntityFactory {
    EntityRegistry& entities_;
    Components& components_;
    const RequiredSystems required_;
    CHUD_Manager* hud;

    static TransformComponent to_transform(const Vec3& location, const Vec3& orientation);
	static TransformComponent to_transform(const Vec3& location, const Vec3& orientation, const Vec3& scaling);

    physx::PxRigidStatic* make_static_box_from_meshes(Entity entity, const std::vector<Entity>& children, const Transform& transform);
    physx::PxRigidDynamic* make_dynamic_box_from_meshes(Entity entity, const std::vector<Entity>& children, const Transform& transform, float mass);
    PM3D::HitBox extract_hitbox_from_meshes(Entity entity, const std::vector<Entity>& children);
    void offset_mesh_if_needed(Entity entity);

public:
    EntityFactory( EntityRegistry& entities, Components& components, const RequiredSystems& required, CHUD_Manager* hud) noexcept;

    // Helper functions
    std::optional<std::vector<Entity>> attach_mesh_to_entity(Entity entity, std::string_view mesh, std::string_view material);
    bool attach_grappling_to_entity(Entity entity, physx::PxRigidActor* body);

    void follow_spline(Entity entity, std::string_view spline, float speed, float initial_value);

    // Environment related objects
    Entity make_simple_decoration(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling);
    Entity make_collidable_decoration(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling);
    Entity make_dynamic_decoration(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, float mass);
    Entity make_variable_color_dynamic_decoration(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, float mass);
    
    Entity make_billboard_decoration(float half_width, float half_height, std::string_view material, Vec3 location, BillboardLinearOrientation orientation, Vec3 scaling);

    Entity make_grapping_box(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling);
    Entity make_invisible_box(Vec3 location, Vec3 orientation, Vec3 scaling);
	Entity make_bridge(std::string_view mesh_wall, std::string_view material_wall, Vec3 location, Vec3 orientation, Vec3 scaling);
	Entity make_building(std::string_view mesh_wall, std::string_view material_wall,
                       std::string_view mesh_roof, std::string_view material_roof, 
                       Vec3 location, Vec3 orientation, Vec3 scaling_wall, Vec3 scaling_roof, int floor_num, float floor_spacing);

    // Player related objects
    Entity make_player(Vec3 location, Vec3 orientation);
    Entity make_camera(Entity target, std::string_view len, Vec3 location, Vec3 orientation);
    Entity make_fps_camera(Entity target, std::string_view len, Vec3 orientation);
    Entity make_camera_manager(Entity first_person_camera, Entity third_person_camera, Entity player);
    
    Entity make_grappling_manager(Entity player_entity, Entity camera_entity);
    Entity make_game_controller();

    // Gameplay related objects
    Entity make_goal(Vec3 half_extends, Vec3 location, Vec3 orientation);
    Entity make_chrono();

    // Make lights
    Entity make_point_light(Couleur diffuse, Couleur specular, Vec3 location, float range, float intensity);
    Entity make_dir_light(Couleur diffuse, Couleur specular, Vec3 orientation, float intensity);
    Entity make_spot_light(Couleur diffuse, Couleur specular, Vec3 location, Vec3 orientation, float angle, float range, float intensity);

    Entity make_meshed_point_light(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, 
                                   Couleur light_color, float range, float intensity);
    Entity make_meshed_spot_light(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, 
                                  Couleur light_color, Vec3 light_direction, float angle, float range, float intensity, Vec3 offset = { 0.f,0.f,0.f });
};

#endif