#ifndef DEF_ENTITY_FACTORY_API_H
#define DEF_ENTITY_FACTORY_API_H

#include "Entity.h"
#include "Gestionnaires.h"
#include "MathHelpers.h"
#include "Couleur.h"
#include "BillboardComponent.h"

class EntityFactory;

namespace api {

Entity make_simple_decoration(EntityFactory*, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling);
Entity make_collidable_decoration(EntityFactory*, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling);
Entity make_dynamic_decoration(EntityFactory*, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, float mass);
Entity make_variable_color_dynamic_decoration(EntityFactory*, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, float mass);
Entity make_billboard_decoration(EntityFactory*, float half_width, float half_height, std::string_view material, Vec3 location, BillboardLinearOrientation orientation, Vec3 scaling);

Entity make_grapping_box(EntityFactory*, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling);
Entity make_invisible_box(EntityFactory*, Vec3 location, Vec3 orientation, Vec3 scaling);
Entity make_bridge(EntityFactory*, std::string_view mesh_wall, std::string_view material_wall, Vec3 location, Vec3 orientation, Vec3 scaling);
Entity make_building(EntityFactory*,std::string_view mesh_wall, std::string_view material_wall, std::string_view mesh_roof, std::string_view material_roof,Vec3 location, Vec3 orientation, Vec3 scaling_wall, Vec3 scaling_roof, int floor_num, float floor_spacing);

Entity make_player(EntityFactory*, Vec3 location, Vec3 orientation);
Entity make_camera(EntityFactory*, Entity target, std::string_view len, Vec3 location, Vec3 orientation);
Entity make_first_person_camera(EntityFactory*, Entity target, std::string_view len_name, Vec3 orientation);
Entity make_grappling_manager(EntityFactory*, Entity player_entity, Entity camera_entity);
Entity make_cameras_manager(EntityFactory*, Entity fp_camera, Entity tp_camera, Entity player);
Entity make_game_controller(EntityFactory*);

Entity make_goal(EntityFactory*, Vec3 half_extends, Vec3 location, Vec3 orientation);
Entity make_chrono(EntityFactory*);

Entity make_point_light(EntityFactory*, Couleur diffuse, Couleur specular, Vec3 location, float range, float intensity);
Entity make_dir_light(EntityFactory*, Couleur diffuse, Couleur specular, Vec3 orientation, float intensity);
Entity make_spot_light(EntityFactory*, Couleur diffuse, Couleur specular, Vec3 location, Vec3 orientation, float angle, float range, float intensity);

Entity make_meshed_point_light(EntityFactory*, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling,
  Couleur light_color, float range, float intensity);
Entity make_meshed_spot_light(EntityFactory*, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling,
  Couleur light_color, Vec3 light_direction, float angle, float range, float intensity, Vec3 offset = { 0.f,0.f,0.f });


void follow_spline(EntityFactory*, Entity e, std::string_view spline, float speed, float initial_t);

struct EntityFactoryApi {
    using make_grappling_manager_fn = Entity(*)(EntityFactory*, Entity, Entity);
    using make_goal_fn = Entity(*)(EntityFactory*, Vec3, Vec3, Vec3);
    using make_point_light_fn = Entity(*)(EntityFactory*, Couleur, Couleur, Vec3, float, float);
    using make_dir_light_fn = Entity(*)(EntityFactory*, Couleur, Couleur, Vec3, float);
    using make_spot_light_fn = Entity(*)(EntityFactory*, Couleur, Couleur, Vec3, Vec3, float, float, float);
    using make_meshed_point_light_fn = Entity(*)(EntityFactory*, std::string_view, std::string_view, Vec3, Vec3, Vec3, Couleur, float, float);
    using make_meshed_spot_light_fn = Entity(*)(EntityFactory*, std::string_view, std::string_view, Vec3, Vec3, Vec3, Couleur, Vec3, float, float, float, Vec3);
    using make_simple_decoration_fn = Entity(*)(EntityFactory*, std::string_view, std::string_view, Vec3, Vec3, Vec3);
    using make_collidable_decoration_fn = Entity(*)(EntityFactory*, std::string_view, std::string_view, Vec3, Vec3, Vec3);
    using make_dynamic_decoration_fn = Entity(*)(EntityFactory*, std::string_view, std::string_view, Vec3, Vec3, Vec3, float);
    using make_variable_color_dynamic_decoration_fn = Entity(*)(EntityFactory*, std::string_view, std::string_view, Vec3, Vec3, Vec3, float);
    using make_camera_fn = Entity(*)(EntityFactory*, Entity, std::string_view, Vec3, Vec3);
    using make_grapping_box_fn = Entity(*)(EntityFactory*, std::string_view, std::string_view, Vec3, Vec3, Vec3);
    using make_invisible_box_fn = Entity(*)(EntityFactory*, Vec3, Vec3, Vec3);
    using make_player_fn = Entity(*)(EntityFactory*, Vec3, Vec3);
    using make_first_person_camera_fn = Entity(*)(EntityFactory*, Entity, std::string_view, Vec3);
    using make_cameras_manager_fn = Entity(*)(EntityFactory*, Entity, Entity, Entity);
	using make_bridge_fn = Entity(*)(EntityFactory*, std::string_view , std::string_view, Vec3 , Vec3 , Vec3 );
	using make_building_fn = Entity(*)(EntityFactory*, std::string_view, std::string_view, std::string_view, std::string_view, Vec3, Vec3, Vec3, Vec3, int, float);
    using make_game_controller_fn = Entity(*)(EntityFactory*);
    using make_chrono_fn = Entity(*)(EntityFactory*);
    using follow_spline_fn = void(*)(EntityFactory*, Entity, std::string_view, float, float);
    using make_billboard_decoration_fn = Entity(*)(EntityFactory*, float, float, std::string_view, Vec3, BillboardLinearOrientation, Vec3);

    make_grappling_manager_fn make_grappling_manager;
    make_goal_fn make_goal;
    make_point_light_fn make_point_light;
    make_dir_light_fn make_dir_light;
    make_spot_light_fn make_spot_light;
    make_meshed_point_light_fn make_meshed_point_light;
    make_meshed_spot_light_fn make_meshed_spot_light;
    make_simple_decoration_fn make_simple_decoration;
    make_collidable_decoration_fn make_collidable_decoration;
    make_dynamic_decoration_fn make_dynamic_decoration;
    make_variable_color_dynamic_decoration_fn make_variable_color_dynamic_decoration;
    make_player_fn make_player;
    make_camera_fn make_camera;
    make_grapping_box_fn make_grapping_box;
    make_invisible_box_fn make_invisible_box;
    make_first_person_camera_fn make_first_person_camera;
    make_cameras_manager_fn make_cameras_manager;
	make_bridge_fn make_bridge;
	make_building_fn make_building;
    make_game_controller_fn make_game_controller;
    make_chrono_fn make_chrono;
    follow_spline_fn follow_spline;
    make_billboard_decoration_fn make_billboard_decoration;

    EntityFactory* factory;

    EntityFactoryApi(EntityFactory* factory);
};

}

#endif