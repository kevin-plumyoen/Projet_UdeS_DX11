#include "EntityFactoryAPI.h"
#include "EntityFactory.h"

namespace api {

Entity make_simple_decoration(EntityFactory* factory, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling) {
    return factory->make_simple_decoration(mesh, material, location, orientation, scaling);
}

Entity make_collidable_decoration(EntityFactory* factory, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling) {
    return factory->make_collidable_decoration(mesh, material, location, orientation, scaling);
}

Entity make_dynamic_decoration(EntityFactory* factory, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, float mass) {
    return factory->make_dynamic_decoration(mesh, material, location, orientation, scaling, mass);
}

Entity make_variable_color_dynamic_decoration(EntityFactory* factory, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, float mass) {
  return factory->make_variable_color_dynamic_decoration(mesh, material, location, orientation, scaling, mass);
}

Entity make_billboard_decoration(EntityFactory* factory, float half_width, float half_height, std::string_view material, Vec3 location, BillboardLinearOrientation orientation, Vec3 scaling) {
    return factory->make_billboard_decoration(half_width, half_height, material, location, orientation, scaling);
}

Entity make_grapping_box(EntityFactory* factory, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling) {
    return factory->make_grapping_box(mesh, material, location, orientation, scaling);
}

Entity make_invisible_box(EntityFactory* factory, Vec3 location, Vec3 orientation, Vec3 scaling) {
    return factory->make_invisible_box(location, orientation, scaling);
}

Entity make_bridge(EntityFactory* factory, std::string_view mesh_wall, std::string_view material_wall, Vec3 location, Vec3 orientation, Vec3 scaling) {
	return factory->make_bridge(mesh_wall, material_wall, location, orientation, scaling);
}

Entity make_building(EntityFactory* factory, std::string_view mesh_wall, std::string_view material_wall,
  std::string_view mesh_roof, std::string_view material_roof,
  Vec3 location, Vec3 orientation, Vec3 scaling_wall, Vec3 scaling_roof, int floor_num, float floor_spacing) {
  return factory->make_building(mesh_wall, material_wall, mesh_roof, material_roof, location, orientation, scaling_wall, scaling_roof, floor_num, floor_spacing);
}

Entity make_player(EntityFactory* factory, Vec3 location, Vec3 orientation) {
    return factory->make_player(location, orientation);
}

Entity make_camera(EntityFactory* factory, Entity target, std::string_view len, Vec3 location, Vec3 orientation) {
    return factory->make_camera(target, len, location, orientation);
}

Entity make_first_person_camera(EntityFactory* factory, Entity target, std::string_view len_name, Vec3 orientation) {
    return factory->make_fps_camera(target, len_name, orientation);
}

Entity make_grappling_manager(EntityFactory* factory, Entity player_entity, Entity camera_entity) {
    return factory->make_grappling_manager(player_entity, camera_entity);
}

Entity make_cameras_manager(EntityFactory* factory, Entity fp_camera, Entity tp_camera, Entity player) {
    return factory->make_camera_manager(fp_camera, tp_camera, player);
}

Entity make_game_controller(EntityFactory* factory) {
    return factory->make_game_controller();
}

Entity make_goal(EntityFactory* factory, Vec3 half_extends, Vec3 location, Vec3 orientation) {
    return factory->make_goal(half_extends, location, orientation);
}

Entity make_chrono(EntityFactory* factory) {
    return factory->make_chrono();
}

Entity make_point_light(EntityFactory* factory, Couleur diffuse, Couleur specular, Vec3 location, float range, float intensity) {
    return factory->make_point_light(diffuse, specular, location, range, intensity);
}

Entity make_dir_light(EntityFactory* factory, Couleur diffuse, Couleur specular, Vec3 orientation, float intensity) {
    return factory->make_dir_light(diffuse, specular, orientation, intensity);
}

Entity make_spot_light(EntityFactory* factory, Couleur diffuse, Couleur specular, Vec3 location, Vec3 orientation, float angle, float range, float intensity) {
    return factory->make_spot_light(diffuse, specular, location, orientation, angle, range, intensity);
}
Entity make_meshed_point_light(EntityFactory* factory, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling,
    Couleur light_color, float range, float intensity) {
    return factory->make_meshed_point_light(mesh, material, location, orientation, scaling, light_color, range, intensity);
}
Entity make_meshed_spot_light(EntityFactory* factory, std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling,
    Couleur light_color, Vec3 light_direction, float angle, float range, float intensity, Vec3 offset) {
    return factory->make_meshed_spot_light(mesh, material, location, orientation, scaling, light_color, light_direction, angle, range, intensity, offset);
}

void follow_spline(EntityFactory* factory, Entity e, std::string_view spline, float speed, float initial_t) {
    factory->follow_spline(e, spline, speed, initial_t);
}

EntityFactoryApi::EntityFactoryApi(EntityFactory* factory)
: make_grappling_manager{api::make_grappling_manager}
, make_goal{api::make_goal}
, make_point_light{api::make_point_light}
, make_dir_light{api::make_dir_light}
, make_spot_light{api::make_spot_light}
, make_meshed_point_light{api::make_meshed_point_light}
, make_meshed_spot_light{api::make_meshed_spot_light}
, make_simple_decoration{api::make_simple_decoration}
, make_collidable_decoration{api::make_collidable_decoration}
, make_dynamic_decoration{api::make_dynamic_decoration}
, make_variable_color_dynamic_decoration{api::make_variable_color_dynamic_decoration}
, make_player{api::make_player}
, make_camera{api::make_camera}
, make_grapping_box{api::make_grapping_box}
, make_invisible_box{api::make_invisible_box}
, make_first_person_camera{api::make_first_person_camera}
, make_cameras_manager{api::make_cameras_manager}
, make_bridge{api::make_bridge}
, make_building{api::make_building}
, make_game_controller{api::make_game_controller}
, make_billboard_decoration{api::make_billboard_decoration}
, follow_spline(api::follow_spline)
, make_chrono{api::make_chrono}
, factory{factory} {

}

}