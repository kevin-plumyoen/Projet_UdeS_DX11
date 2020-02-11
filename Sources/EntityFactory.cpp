#include "EntityFactory.h"

#include "EntityRegistry.h"
#include "PhysicHelpers.h"
#include "RequiredSystems.h"
#include "PhysicEngine.h"
#include "CollisionMasks.h"
#include "CameraController.h"
#include "WreckingBallCameraController.h"
#include "Identities.h"
#include "MouseGrapplingController.h"
#include "GrapplingManager.h"
#include "HitBox.h"
#include "Logger.h"
#include "PxPhysicsAPI.h"
#include "Components.h"
#include "GameplayCameraManager.h"
#include "KeyboardCameraChangeController.h"
#include "FirstPersonCameraController.h"
#include "PlayerController.h"
#include "BridgeController.h"
#include "GameController.h"
#include "ChronoManager.h"

EntityFactory::EntityFactory(EntityRegistry& entities, Components& components, const RequiredSystems& systems, CHUD_Manager* hud) noexcept
: entities_{ entities }
, components_{ components }
, hud{ hud }
, required_{ systems } {

}

#pragma region Helpers
static void attach_entity_to_rigidbody(const Entity& entity, PxRigidActor* actor) {
    intptr_t entity_to_ptr = 0;
    std::memcpy(&entity_to_ptr, &entity, sizeof(Entity));
    ++entity_to_ptr;
    PhysicHelper::attach_user_data(actor, reinterpret_cast<void*>(entity_to_ptr));
}

TransformComponent EntityFactory::to_transform(const Vec3& location, const Vec3& orientation) {
    return to_transform(location, orientation, Vec3{ 1.f, 1.f, 1.f });
}

TransformComponent EntityFactory::to_transform(const Vec3& location, const Vec3& orientation, const Vec3& scaling) {
    TransformComponent transform;
    transform.local.translation = location;
    transform.local.rotation = DirectX::XMQuaternionRotationRollPitchYaw(orientation.x, orientation.y, orientation.z);
    transform.local.scaling = scaling;

    return transform;
}

std::optional<std::vector<Entity>> EntityFactory::attach_mesh_to_entity(Entity entity, std::string_view mesh, std::string_view material) {
    const bool use_imported_material = material == "_IMPORTED_";

    if (entity.valid()) {
        const std::vector<MeshPtr> meshes = required_.meshes.at(std::string{mesh});

        // Si un seul mesh, on l'attache directement à new_entity
        if(meshes.size() == 1) {
            StaticMeshComponent mesh_component;
            if(use_imported_material) {
                mesh_component.material = required_.imported_materials.at(std::string{mesh}).front();
            }
            else {
                mesh_component.material = required_.materials.at(std::string{material});
            }
            mesh_component.mesh = std::make_shared<PM3D::CMeshImporter>(*required_.meshes.at(std::string{mesh})[0], 
                                                                        &required_.dispositif);

            components_.static_meshes.attach(entity, std::move(mesh_component));

            return std::vector<Entity>{};
        }
        // Sinon, les meshes sont attachés à des enfants de new_entity
        else {
            std::vector<Entity> child_entities;
            child_entities.reserve(meshes.size());
            entities_.create(meshes.size(), std::back_inserter(child_entities));

            // Si un enfant n'est pas valide
            if (std::any_of(child_entities.begin(), child_entities.end(), [](const Entity e) {return !e.valid(); })) {
                // On supprime tous les enfants
                entities_.destroy(child_entities.begin(), child_entities.end());
                return {};
            }

            for (std::size_t child_index = 0; child_index < meshes.size(); ++child_index) {
                StaticMeshComponent child_mesh_component;
                if (use_imported_material) {
                    child_mesh_component.material = required_.imported_materials.at(std::string{mesh})[child_index];
                }
                else {
                    child_mesh_component.material = required_.materials.at(std::string{material}); // Use suggested mesh material
                }
                child_mesh_component.mesh = std::make_shared<PM3D::CMeshImporter>(*required_.meshes.at(std::string{mesh})[child_index], 
                                                                            &required_.dispositif);

                components_.transforms.attach(child_entities[child_index], TransformComponent{});
                components_.static_meshes.attach(child_entities[child_index], std::move(child_mesh_component));

                entities_.set_link(entity, child_entities[child_index]);
            }

            return child_entities;
        }
    }

    return {};
}

bool EntityFactory::attach_grappling_to_entity(Entity entity, physx::PxRigidActor* body) {
    if (entity.valid()) {

        GrapplingComponent grap_component;
		PhysicHelper::set_query_filter_mask(body, CollisionMasks::CameraObstacleBit | grap_component());
        components_.grapplings.attach(entity, std::move(grap_component));

        return true;
    }

    return false;
}

namespace {

class SplineFollowingController {
    SplineStorage& splines;
    Entity entity;
    float speed;

public:
    SplineFollowingController(Entity entity, SplineStorage& splines, float speed)
    : splines{ splines }
    , entity{ entity }
    , speed{speed} {

    }
    
    void update(FrameDuration dt) {
        std::chrono::duration<float> fsecs = dt;

        auto spline_it = splines.find(entity);

        if (spline_it != splines.end()) {
            float new_t = std::fmod(spline_it->second.t + fsecs.count() * speed, spline_it->second.spline.length());

            spline_it->second.t = new_t;
        }
    }
};

void follow_spline_update_callback(FrameDuration dt, uint8_t, void* userdata) {
	SplineFollowingController* ctrl = static_cast<SplineFollowingController*>(userdata);

	ctrl->update(dt);
}

void follow_spline_free_callback(void* userdata) {
	SplineFollowingController* ctrl = static_cast<SplineFollowingController*>(userdata);

	delete ctrl;
}
}

void EntityFactory::follow_spline(Entity entity, std::string_view spline, float speed, float initial_value) {
    if (entity.valid()) {
		SplineComponent component;
		if (components_.transforms.is_attached(entity)) {
			component = SplineComponent(required_.splines.at(std::string{ spline }), components_.transforms.find(entity)->second.local);

		}
		else {
			component= SplineComponent(required_.splines.at(std::string{ spline }));
		}

		component.t = initial_value;
        components_.splines.attach(entity, std::move(component));

        Entity update_child = entities_.create();
        SplineFollowingController* following_ctrl = new SplineFollowingController(entity, components_.splines, speed);
        components_.updates.attach(update_child, UpdateComponent(follow_spline_update_callback, UpdateFlags::UpdateBit, following_ctrl, follow_spline_free_callback));
    }
}

#pragma endregion

#pragma region Environment
Entity EntityFactory::make_simple_decoration(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling) {
    Entity new_entity = entities_.create();

    if (new_entity.valid()) {
        TransformComponent transform = to_transform(location, orientation, scaling);
        components_.transforms.attach(new_entity, std::move(transform));

        // Si on échoue d'attacher le mesh à l'entité on annule tout
        if (!attach_mesh_to_entity(new_entity, mesh, material)) {
            log_critical("game.entity_factory", "failed to attach mesh to entity");
            entities_.destroy(new_entity);
            return Entity{};
        }

        offset_mesh_if_needed(new_entity);
    }

    return new_entity;
}

physx::PxRigidStatic* EntityFactory::make_static_box_from_meshes(Entity entity, const std::vector<Entity>& children, const Transform& transform) {
    const PM3D::HitBox hitbox = extract_hitbox_from_meshes(entity, children);

    physx::PxRigidStatic* body = nullptr;
    
    body = required_.physic.createStaticCube(PhysicMaterial{ 0.5f, 0.5f, 0.5f, 100.f },
                                            transform.translation, transform.eulerAngles(), transform.scale(hitbox.scaleBox));
	RigidBodyComponent rigid_body_component;
	rigid_body_component.rigidbody = body;
	components_.rigidbodies.attach(entity, std::move(rigid_body_component));

    return body;
}

physx::PxRigidDynamic* EntityFactory::make_dynamic_box_from_meshes(Entity entity, const std::vector<Entity>& children, const Transform& transform, float mass) {
    const PM3D::HitBox hitbox = extract_hitbox_from_meshes(entity, children);

    physx::PxRigidDynamic* body = nullptr;
    
    body = required_.physic.createDynamicCube(PhysicMaterial{ 0.5f, 0.5f, 0.5f, mass },
                                              transform.translation, transform.eulerAngles(), transform.scale(hitbox.scaleBox));
	RigidBodyComponent rigid_body_component;
	rigid_body_component.rigidbody = body;
    rigid_body_component.updateTransform = true;
	components_.rigidbodies.attach(entity, std::move(rigid_body_component));
    
    return body;
}

PM3D::HitBox EntityFactory::extract_hitbox_from_meshes(Entity entity, const std::vector<Entity>& children) {
    PM3D::HitBox hitbox;

    if (children.size() == 0) {
        auto mesh_it = components_.static_meshes.find(entity);

        hitbox = mesh_it->second.mesh->BoundingBox();
    }
    // Combine all hitboxes
    else {
        for (Entity child_entity : children) {
            auto mesh_it = components_.static_meshes.find(child_entity);
            hitbox = hitbox + mesh_it->second.mesh->BoundingBox();
        }
    }

    return hitbox;
}

void EntityFactory::offset_mesh_if_needed(Entity entity) {
    // On retrouve les enfants directe de l'entité
    std::vector<Entity> children;
    entities_.all_children(entity, std::back_inserter(children));

    const PM3D::HitBox hitbox = extract_hitbox_from_meshes(entity, children);
    auto static_mesh_it = components_.static_meshes.find(entity);

    // Pas besoin de offset le mesh
    if (hitbox.centre.x < 0.1f && hitbox.centre.y < 0.1f && hitbox.centre.z < 0.1f) {
        return;
    }

    // Pas de mesh trouvé, alors c'est les enfants!
    // C'est beaucoup plus, facile
    if (static_mesh_it == components_.static_meshes.end()) {
        auto offset_entity = entities_.create();

        TransformComponent transform;
        transform.local.translation = Vec3{-hitbox.centre.x, -hitbox.centre.y, -hitbox.centre.z};

        components_.transforms.attach(offset_entity, std::move(transform));

        // On déplace tous les enfants dans offset entity
        for (Entity child : children) {
            entities_.reset_link(entity, child);
            entities_.set_link(offset_entity, child);
        }

        entities_.set_link(entity, offset_entity);

    }
    // Un mesh trouvé, on crée un enfant, y déplace le mesh + transform de offset
    else {
        auto mesh_it = components_.static_meshes.find(entity);

        auto offset_entity = entities_.create();

        TransformComponent transform;
        transform.local.translation = hitbox.centre;

        components_.transforms.attach(offset_entity, std::move(transform));
        components_.static_meshes.attach(offset_entity, mesh_it->second);
        components_.static_meshes.detach(entity);
    }
}

Entity EntityFactory::make_collidable_decoration(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling) {
    Entity new_entity = entities_.create();

    if (new_entity.valid()) {
        TransformComponent transform = to_transform(location, orientation, scaling);
        components_.transforms.attach(new_entity, std::move(transform));

        auto child_entities = attach_mesh_to_entity(new_entity, mesh, material);

        if (!child_entities) {
            log_critical("game.entity_factory", "failed to attach mesh to entity");
            entities_.destroy(new_entity);
            return Entity{};
        }

        PxRigidStatic* body = make_static_box_from_meshes(new_entity, *child_entities, transform.local);
		RigidBodyComponent rigid_body_component;
        rigid_body_component.updateTransform = true;
		rigid_body_component.rigidbody = body;
		components_.rigidbodies.attach(new_entity, std::move(rigid_body_component));

        offset_mesh_if_needed(new_entity);
    }

    return new_entity;
}

Entity EntityFactory::make_dynamic_decoration(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, float mass) {
    Entity new_entity = entities_.create();

    if (new_entity.valid()) {
        TransformComponent transform = to_transform(location, orientation, scaling);
        components_.transforms.attach(new_entity, std::move(transform));

        auto child_entities = attach_mesh_to_entity(new_entity, mesh, material);

        if (!child_entities) {
            log_critical("game.entity_factory", "failed to attach mesh to entity");
            entities_.destroy(new_entity);
            return Entity{};
        }

        PxRigidDynamic* body = make_dynamic_box_from_meshes(new_entity, *child_entities, transform.local, mass);
		RigidBodyComponent rigid_body_component;
		rigid_body_component.rigidbody = body;
		components_.rigidbodies.attach(new_entity, std::move(rigid_body_component));

        offset_mesh_if_needed(new_entity);

        attach_grappling_to_entity(new_entity, body);
    }

    return new_entity;
}

Entity EntityFactory::make_variable_color_dynamic_decoration(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, float mass) {
  Entity new_entity = make_dynamic_decoration(mesh, material, location, orientation, scaling, mass);

  components_.variable_colors.attach(new_entity, VariableColorComponent{});

  return new_entity;
}

Entity EntityFactory::make_billboard_decoration(float half_width, float half_height, std::string_view material, Vec3 location, BillboardLinearOrientation orientation, Vec3 scaling) {
    Entity new_entity = entities_.create();

    if (new_entity.valid()) {
        TransformComponent transform = to_transform(location, Vec3{0.f, 0.f, 0.f}, scaling);
        components_.transforms.attach(new_entity, std::move(transform));

        BillboardComponent billboard;
        billboard.material = required_.materials.at(std::string{material});
        billboard.orientation = orientation;
        billboard.billboard = std::make_shared<Billboard>(Vec2{half_width, half_height}, &required_.dispositif);

        components_.billboards.attach(new_entity, std::move(billboard));
    }

    return new_entity;
}

struct Light_bridge {
	EntityRegistry* entities;
	std::vector<Entity> lights;
	Light_bridge(std::vector<Entity> lights_, EntityRegistry* entities_) : lights(lights_), entities(entities_) {};

	static void liberer(void* light_ptr) {
		Light_bridge* lb = static_cast<Light_bridge*>(light_ptr);
		delete lb;
	}
};

namespace {
	void bridge_callback(FrameDuration dt, uint8_t, void* userdata) {
		BridgeController* ctrl = static_cast<BridgeController*>(userdata);

		ctrl->update(dt);
	}

	void bridge_free_callback(void* userdata) {
		BridgeController* ctrl = static_cast<BridgeController*>(userdata);

		delete ctrl;
	}
}

Entity EntityFactory::make_bridge(std::string_view mesh_wall, std::string_view material_wall, Vec3 global_location, Vec3 global_orientation, Vec3 global_scaling) {
	Entity bridge = entities_.create();
	if (bridge.valid()) {
        TransformComponent parent_transform = to_transform(global_location, global_orientation, global_scaling);
		components_.transforms.attach(bridge, parent_transform);

		std::shared_ptr<Mesh> p_mesh_wall = required_.meshes.at(std::string{ mesh_wall })[0];

        TransformComponent local_transforms[3] = { to_transform(Vec3{ -5.f, 0.f, 0.f }, Vec3{ 0.f, 0.f,                0.f }, Vec3{ 1.f, 1.f, 1.0f }),
                                                   to_transform(Vec3{  5.f, 0.f, 0.f }, Vec3{ 0.f, 0.f,                0.f }, Vec3{ 1.f, 1.f, 1.0f }),
                                                   to_transform(Vec3{  0.f, 0.f, 5.f }, Vec3{ 0.f, -DirectX::XM_PIDIV2, 0.f }, Vec3{ 1.f, 1.f, 1.5f }) };

        Entity walls[3];
        entities_.create(3, &walls[0]);
        for(int i = 0; i < 3; ++i) {
            entities_.set_link(bridge, walls[i]);

            components_.transforms.attach(walls[i], local_transforms[i]);

            const Transform global_transform = TransformHierarchy::apply_parent_transform(parent_transform.local, local_transforms[i].local);

            PM3D::HitBox hitbox = PM3D::HitBox::hitbox_from_mesh(*p_mesh_wall);
			PxRigidStatic* body = required_.physic.createStaticCube(PhysicMaterial{ 0.5f, 0.5f, 0.5f, 100.f },
                                                                    global_transform.translation, 
                                                                    global_transform.eulerAngles(), 
                                                                    global_transform.scale(hitbox.scaleBox));
			RigidBodyComponent rigid_body_component;
			rigid_body_component.rigidbody = body;
			components_.rigidbodies.attach(walls[i], std::move(rigid_body_component));

            attach_grappling_to_entity(walls[i], body);
            if (!attach_mesh_to_entity(walls[i], mesh_wall, material_wall)) {
                log_critical("entity_factory", "failed to attach static mesh to left wall");
            }
        }

        // Ajout les lumières

		std::vector<Entity> lights;
		for (int i = 0; i < 5; i++) {
            const Vec3 local_translation(0.f, 0.f -6.f + i * 3.f, 0.f);
			lights.push_back(make_point_light(Couleur(0.0f, 0.0f, 0.8f, 1.0f), Couleur(0.0f, 0.0f, 8.0f, 1.0f), local_translation, 25.0f, 1.0f));
			
            entities_.set_link(bridge, lights[i]);
			entities_.disable(lights[i]);
		}


        Light_bridge* lb = new Light_bridge(lights, &entities_);
		TriggerComponent trigger(Identities::Tunnel, lb, Light_bridge::liberer);
		components_.triggers.attach(bridge, std::move(trigger));

		BridgeController* bridgeController = new BridgeController(lights, entities_, required_.tags);
		UpdateComponent update(bridge_callback, UpdateFlags::LateUpdateBit, bridgeController, bridge_free_callback);
		components_.updates.attach(bridge, std::move(update));

        const Vec3 trigger_scale_box = parent_transform.local.scale(Vec3{4.f, 8.f, 4.f});

		PxRigidStatic* trigger_box_rigid_static = required_.physic.createStaticTriggerBox(trigger_scale_box, PhysicMaterial(1.f, 1.f, 1.f), global_location, global_orientation);
		attach_entity_to_rigidbody(bridge, trigger_box_rigid_static);
		RigidBodyComponent rigidbody(trigger_box_rigid_static);
		components_.rigidbodies.attach(bridge, std::move(trigger_box_rigid_static));
	}

	return bridge;
}

Entity EntityFactory::make_building(std::string_view mesh_wall, std::string_view material_wall,
                                    std::string_view mesh_roof, std::string_view material_roof,
                                    Vec3 location, Vec3 orientation, Vec3 scaling_wall, Vec3 scaling_roof, int floor_num, float floor_spacing) {
  Entity building = entities_.create();
  for (int i = 0; i < floor_num; ++i) {
    Vec3 new_loc = Vec3{ location.x, location.y, location.z + floor_spacing * i };
    Entity floor = make_grapping_box(mesh_wall, material_wall, new_loc, orientation, scaling_wall);
    entities_.set_link(building,floor);
  }
  Vec3 new_loc = Vec3{ location.x, location.y, location.z + floor_spacing * (floor_num - 0.5f) };
  Entity floor = make_grapping_box(mesh_roof, material_roof, new_loc, orientation, scaling_roof);
  entities_.set_link(building, floor);

  return building;
}


Entity EntityFactory::make_grapping_box(std::string_view mesh_name, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling) {
    Entity new_entity = entities_.create();

    if (new_entity.valid()) {
        std::shared_ptr<Mesh> mesh = required_.meshes.at(std::string{mesh_name})[0];

        TransformComponent transform = to_transform(location, orientation, scaling);

        PM3D::HitBox hitbox = PM3D::HitBox::hitbox_from_mesh(*mesh);
        hitbox.transform(transform.local);
        orientation.x = -orientation.x;
        float y = orientation.y;
        orientation.y = -orientation.z;
        orientation.z = -y;

        PxRigidStatic* cube_body = required_.physic.createStaticCube(PhysicMaterial{ 0.5f, 0.5f, 0.5f, 100.f }, 
                                                                     hitbox.centre, orientation, hitbox.scaleBox);

        RigidBodyComponent rigid_body_component_cube;
        rigid_body_component_cube.rigidbody = cube_body;
        
        GrapplingComponent grappling_component_cube;
        PhysicHelper::set_query_filter_mask(cube_body, CollisionMasks::CameraObstacleBit | grappling_component_cube());

        components_.transforms.attach(new_entity, std::move(transform));
        components_.rigidbodies.attach(new_entity, std::move(rigid_body_component_cube));
        components_.grapplings.attach(new_entity, std::move(grappling_component_cube));

		attach_mesh_to_entity(new_entity, mesh_name, material);
    }

    return new_entity;
}

Entity EntityFactory::make_invisible_box(Vec3 location, Vec3 orientation, Vec3 scaling) {
    Entity new_entity = entities_.create();

    if (new_entity.valid()) {
        TransformComponent transform = to_transform(location, orientation, scaling);

        PxRigidStatic* cube_body = required_.physic.createStaticCube(PhysicMaterial{ 0.5f, 0.5f, 0.5f, 100.f }, 
                                                                     location, orientation, scaling);

        RigidBodyComponent rigid_body_component_cube;
        rigid_body_component_cube.rigidbody = cube_body;
        
        components_.transforms.attach(new_entity, std::move(transform));
        components_.rigidbodies.attach(new_entity, std::move(rigid_body_component_cube));
    }

    return new_entity;
}

#pragma endregion

#pragma region Player
namespace {
    void player_callback(FrameDuration dt, uint8_t update_flags, void* userdata) {
        PlayerController* ctrl = static_cast<PlayerController*>(userdata);

        if((update_flags & UpdateFlags::UpdateBit) != 0) {
            ctrl->update(dt);
        }

        if ((update_flags & UpdateFlags::LateUpdateBit) != 0) {
            ctrl->late_update(dt);
        }
    }

    void player_free_callback(void* userdata) {
        PlayerController* ctrl = static_cast<PlayerController*>(userdata);

        delete ctrl;
    }
}

Entity EntityFactory::make_player(Vec3 location, Vec3 orientation) {
    static const float PlayerMass = 100.f;

    Entity new_entity = entities_.create();

    if (new_entity.valid()) {
        TransformComponent transform = to_transform(location, orientation);

        // Physic initialization
        auto sphere_rigidbody = required_.physic.createDynamicSphere(1.f, PhysicMaterial(0.f, 0.8f, 0.5f, PlayerMass), location, orientation);
        PhysicHelper::set_query_filter_mask(sphere_rigidbody, CollisionMasks::PlayerBit);
        attach_entity_to_rigidbody(new_entity, sphere_rigidbody);

        RigidBodyComponent rigidbody(sphere_rigidbody);
        rigidbody.updateTransform = true;

        InputComponent input_component(required_.input_controllers.at("player_movement"));


        PlayerController* playerController = new PlayerController{ new_entity, 
                                                                   hud,
                                                                   required_.postFXs,
                                                                   required_.materials, 
                                                                   components_.rigidbodies,
                                                                   components_.static_meshes, 
                                                                   entities_};

        UpdateComponent update(player_callback, UpdateFlags::LateUpdateBit | UpdateFlags::UpdateBit, playerController, player_free_callback);

        if (!attach_mesh_to_entity(new_entity, "player", "_IMPORTED_")) {
            log_critical("game.entity_factory", "failed to attach mesh to player");
            entities_.destroy(new_entity);
            new_entity = Entity{};
            return Entity{};
        }


        LightComponent light_component;
        light_component.diffuseColor = Couleur(0.080220f, 0.8f, 0.291771f, 1.0f);
        light_component.specularColor = Couleur(0.080220f, 0.8f, 0.291771f, 1.0f);
        light_component.intensity = 1.0f;

        light_component.light = PointLight{ 3.0f };

        components_.rigidbodies.attach(new_entity, std::move(rigidbody));
        components_.transforms.attach(new_entity, std::move(transform));
        components_.inputs.attach(new_entity, std::move(input_component));
        components_.updates.attach(new_entity, std::move(update));
        components_.lights.attach(new_entity,std::move(light_component));

        required_.tags[new_entity] = Identities::Player;
    }

    return new_entity;
}

#pragma endregion

#pragma region Cameras

namespace {
    void player_camera_update_callback(FrameDuration dt, uint8_t, void* userdata) {
        BaseCameraController* ctrl = static_cast<BaseCameraController*>(userdata);

        ctrl->update(dt);
    }

    void player_camera_free_callback(void* userdata) {
        BaseCameraController* ctrl = static_cast<BaseCameraController*>(userdata);

        delete ctrl;
    }
}

Entity EntityFactory::make_camera(Entity target, std::string_view len, Vec3 location, Vec3 orientation) {
    Entity camera_entity = entities_.create();

    if (camera_entity.valid()) {
        TransformComponent transform = to_transform(location, orientation);
        CameraComponent camera(required_.camera_lens.at(std::string{ len }));
        camera.active = true;

        BaseCameraController* controller = new WreckingBallCameraController{camera_entity, target,
                                                                            components_.transforms, required_.transforms_hierarchy,
                                                                            required_.physic };

        UpdateComponent update(player_camera_update_callback, UpdateFlags::LateUpdateBit, controller, player_camera_free_callback);

        // On attache tous les composants à la caméra
        components_.transforms.attach(camera_entity, std::move(transform));
        components_.cameras.attach(camera_entity, std::move(camera));
        components_.updates.attach(camera_entity, std::move(update));
    }

    return camera_entity;
}

Entity EntityFactory::make_fps_camera(Entity target, std::string_view len, Vec3 orientation) {
    if (!target.valid()) {
        log_critical("game.entity_factory.first_person_camera", "missing valid target entity");
        return Entity{};
    }

    Entity camera_entity = entities_.create();

    if (camera_entity.valid()) {
        entities_.set_link(target, camera_entity);

        TransformComponent transform = to_transform(Vec3{0.f, 0.f, 0.f}, orientation);
        CameraComponent camera(required_.camera_lens.at(std::string{ len }));

        BaseCameraController* controller = new FirstPersonCameraController{camera_entity, components_.transforms, required_.transforms_hierarchy};
        UpdateComponent update(player_camera_update_callback, UpdateFlags::LateUpdateBit, controller, player_camera_free_callback);

        components_.transforms.attach(camera_entity, std::move(transform));
        components_.cameras.attach(camera_entity, std::move(camera));
        components_.updates.attach(camera_entity, std::move(update));
    }

    return camera_entity;
}

namespace {
    void camera_manager_update_cb(FrameDuration dt, uint8_t, void* userdata) {
        GameplayCameraManager* manager = static_cast<GameplayCameraManager*>(userdata);
        manager->update(dt);
    }

    void camera_manager_free_cb(void* userdata) {
        GameplayCameraManager* manager = static_cast<GameplayCameraManager*>(userdata);
        delete manager;
    }
}

Entity EntityFactory::make_camera_manager(Entity first_person_camera, Entity third_person_camera, Entity player) {
    if (!first_person_camera.valid()) {
        log_critical("game.entity_factory.camera_manager", "missing valid first person camera entity");
        return Entity{};
    }

    if (!third_person_camera.valid()) {
        log_critical("game.entity_factory.camera_manager", "missing valid third person camera entity");
        return Entity();
    }
    
    Entity manager_entity = entities_.create();

    if (manager_entity.valid()) {
        GameplayCameraManager* manager = new GameplayCameraManager(first_person_camera, third_person_camera, player, components_.transforms, required_.transforms_hierarchy, components_.cameras, components_.rigidbodies);

        components_.updates.attach(manager_entity, UpdateComponent(camera_manager_update_cb, UpdateFlags::UpdateBit, manager, camera_manager_free_cb));
        components_.inputs.attach(manager_entity, InputComponent(std::make_shared<KeyboardCameraChangeController>(&required_.inputs)));
    }

    return manager_entity;
}

#pragma endregion

#pragma region Grapplings

namespace {
    void grappling_manager_update_cb(FrameDuration dt, uint8_t, void* userdata) {
        GrapplingManager* manager = static_cast<GrapplingManager*>(userdata);

        manager->update(dt);
    }

    void grappling_manager_free_cb(void* userdata) {
        GrapplingManager* manager = static_cast<GrapplingManager*>(userdata);
        delete manager;
    }
}

Entity EntityFactory::make_grappling_manager(Entity player_entity, Entity camera_entity) {
    if (!player_entity.valid()) {
        log_critical("game.entity_factory.grappling_manager", "missing valid player entity");
        return Entity{};
    }

    if (!camera_entity.valid()) {
        log_critical("game.entity_factory.grappling_manager", "missing valid camera manager entity");
        return Entity();
    }

    Entity manager_entity = entities_.create();

    if (manager_entity.valid()) {
        GrapplingManager* manager = new GrapplingManager(player_entity, camera_entity, required_.physic, required_.renderer, components_.transforms, components_.updates, components_.cameras, components_.rigidbodies, required_.materials["grappling_cable"], &required_.dispositif);

        components_.updates.attach(manager_entity, UpdateComponent(grappling_manager_update_cb, UpdateFlags::UpdateBit, manager, grappling_manager_free_cb));
        components_.inputs.attach(manager_entity, InputComponent(std::make_shared<MouseGrapplingController>(&required_.inputs)));
    }

    return manager_entity;
}

namespace {
    void game_manager_update_cb(FrameDuration, uint8_t, void* userdata) {
        GameController* manager = static_cast<GameController*>(userdata);

        manager->check_win_condition();
    }

    void game_manager_free_cb(void* userdata) {
        GameController* manager = static_cast<GameController*>(userdata);
        delete manager;
    }
}

Entity EntityFactory::make_game_controller() {
    Entity controller_entity = entities_.create();

    if (controller_entity.valid()) {
        GameController* ctrl = new GameController(required_.tags);

        components_.updates.attach(controller_entity, UpdateComponent(game_manager_update_cb, UpdateFlags::LateUpdateBit, ctrl, game_manager_free_cb));
    }

    return controller_entity;
}

#pragma endregion

#pragma region Gameplay specifics
Entity EntityFactory::make_goal(Vec3 half_extends, Vec3 location, Vec3 orientation) {
    Entity goal_entity = entities_.create();

    if (goal_entity.valid()) {
        TransformComponent transform = to_transform(location, orientation);

        PxRigidStatic* goal_box_rigid_static = required_.physic.createStaticTriggerBox(half_extends, PhysicMaterial(1.f, 1.f, 1.f), location, orientation);
        attach_entity_to_rigidbody(goal_entity, goal_box_rigid_static);

        RigidBodyComponent rigidbody(goal_box_rigid_static);

        TriggerComponent trigger(Identities::Goal);

        components_.transforms.attach(goal_entity, std::move(transform));
        components_.rigidbodies.attach(goal_entity, std::move(goal_box_rigid_static));
        components_.triggers.attach(goal_entity, std::move(trigger));
    }

    return goal_entity;
}

namespace {
    void chrono_update_cb(FrameDuration dt, uint8_t, void* userdata) {
        ChronoManager* manager = static_cast<ChronoManager*>(userdata);

        manager->update(dt);
    }

    void chrono_free_cb(void* userdata) {
        ChronoManager* manager = static_cast<ChronoManager*>(userdata);
        delete manager;
    }
}

Entity EntityFactory::make_chrono() {
    Entity entity = entities_.create();

    if (entity.valid()) {
        ChronoManager* ctrl = new ChronoManager(*hud);

        components_.updates.attach(entity, UpdateComponent(chrono_update_cb, UpdateFlags::UpdateBit, ctrl, chrono_free_cb));
    }

    return entity;
}

#pragma endregion

#pragma region Lights
Entity EntityFactory::make_point_light(Couleur diffuse, Couleur specular, Vec3 location, float range, float intensity) {
    Entity light_entity = entities_.create();

    if (light_entity.valid()) {
        LightComponent light_component;
        light_component.diffuseColor = diffuse;
        light_component.specularColor = specular;
        light_component.intensity = intensity;

        light_component.light = PointLight{ range };

        TransformComponent transform = to_transform(location, Vec3{ 0.0f,0.0f,0.0f });

        components_.lights.attach(light_entity, std::move(light_component));
        components_.transforms.attach(light_entity, std::move(transform));
    }

    return light_entity;
}

Entity EntityFactory::make_dir_light(Couleur diffuse, Couleur specular, Vec3 orientation, float intensity) {
    Entity light_entity = entities_.create();

    if (light_entity.valid()) {
        LightComponent light_component;
        light_component.diffuseColor = diffuse;
        light_component.specularColor = specular;
        light_component.intensity = intensity;

        light_component.light = DirectionalLight{ orientation };

        components_.lights.attach(light_entity, std::move(light_component));
    }

    return light_entity;
}

Entity EntityFactory::make_spot_light(Couleur diffuse, Couleur specular, Vec3 location, Vec3 orientation, float angle, float range, float intensity) {
    Entity light_entity = entities_.create();

    if (light_entity.valid()) {
        LightComponent light_component;
        light_component.diffuseColor = diffuse;
        light_component.specularColor = specular;
        light_component.intensity = intensity;

        light_component.light = SpotLight{ angle,range,orientation };

        TransformComponent transform = to_transform(location, Vec3{ 1.0f,1.0f,1.0f });

        components_.lights.attach(light_entity, std::move(light_component));
        components_.transforms.attach(light_entity, std::move(transform));
    }

    return light_entity;
}

Entity EntityFactory::make_meshed_point_light(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling, 
                                              Couleur light_color, float range, float intensity) {

    auto entity = make_simple_decoration(mesh, material, location, orientation, scaling);

    LightComponent light_component;
    light_component.diffuseColor = light_color;
    light_component.specularColor = light_color;
    light_component.intensity = intensity;

    light_component.light = PointLight{ range };

    components_.lights.attach(entity, light_component);

    return entity;
}

Entity EntityFactory::make_meshed_spot_light(std::string_view mesh, std::string_view material, Vec3 location, Vec3 orientation, Vec3 scaling,
  Couleur light_color, Vec3 light_direction, float angle, float range, float intensity, Vec3 offset) {

    auto entity = make_simple_decoration(mesh, material, location, orientation, scaling);

    LightComponent light_component;
    light_component.diffuseColor = light_color;
    light_component.specularColor = light_color;
    light_component.intensity = intensity;
    light_component.offset = offset;

    light_component.light = SpotLight{ angle,range,light_direction };

    components_.lights.attach(entity, light_component);

    return entity;

}

#pragma endregion