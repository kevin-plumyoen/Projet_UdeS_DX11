#ifndef DEF_GAME_H
#define DEF_GAME_H

#include "EngineContext.h"

// Rendu
#include "Camera.h"
#include "CameraManager.h"
#include "objet3d.h"
#include "Scene.h"
#include "TransformHierarchy.h"

// PhysX
class PhysicEngine;

// ECS
#include "Systems.h"
#include "Components.h"

// Clock
#include "Clock.h"

#include "ControllerLayout.h"
#include "HUD_Manager.h"

#include "ForwardCommand.h"
#include "BackwardCommand.h"
#include "RightCommand.h"
#include "LeftCommand.h"
#include "JumpCommand.h"

#include "DebugCameraController.h"

#include "ScreenBillboard.h"
#include "AfficheurTexte.h"

#include "EntityFactory.h"

#include <vector>
#include <memory>

class Camera;
struct CameraLen;
struct BaseSystem;
struct Renderer;

class RadialBlur;
class Bloom;

class CameraController;

struct aiScene;

namespace Assimp {

class Importer;

}

class MaterielTerrain;
class ShaderTerrain;

namespace PM3D {

class CDIManipulateur;
class CObjet3D;
class CDispositifD3D11;



// Ne sert à rien sauf bien indiqué que la fonction ne peut pas être renommée
// car elle est appelée par le moteur
#define ENGINE_CALLBACK

class Game {
    // Rendering
    PM3D::CDispositifD3D11& dispositif;
    PM3D::CGestionnaireDeTextures& textures;
    GestionnaireShader& shaders;
    GestionnaireMateriel& materiels;
    PM3D::CDIManipulateur& inputs;
    GestionnaireMesh meshes;
    GestionnaireMaterielImporte imported_materials;
    GestionnaireCameraLen camera_lens;
    GestionnaireInputController input_controllers;
    GestionnairePostFX postFXs;
    GestionnaireTag tags;
    GestionnaireSpline splines;

    std::unique_ptr<Font> font;
    std::unique_ptr<CAfficheurTexte> pAfficheurTexte;

    bool quitter;
    bool paused;
    bool transmit_pause_changed;

    std::unique_ptr<Renderer> renderer;

    // Physic
    PhysicEngine& physic;

    // HUD Manager
    std::unique_ptr<CHUD_Manager> hud;

    // ECS related variables
    EntityRegistry entities;
    Components components;
    Scene scene;
    TransformHierarchy transforms_hierarchy;
    Systems systems;
    EntityFactory entity_factory;

    // Terrain
    std::shared_ptr<MaterielTerrain> terrain_material_z1;
    std::shared_ptr<MaterielTerrain> terrain_material_z2;
    std::shared_ptr<MaterielTerrain> terrain_material_z3;
    std::shared_ptr<MaterielTerrain> terrain_material_z4;
    std::shared_ptr<ShaderTerrain> terrain_shader;
    std::vector<std::unique_ptr<CObjet3D>> terrain_patches;
    uint32_t patch_width, patch_depth;
    float tile_size;

    // Skybox
    std::shared_ptr<Materiel> skybox_material;
    std::unique_ptr<CObjet3D> skybox_object;

    // Reticule
    std::unique_ptr<CScreenBillboard> reticule_billboard;
    std::unique_ptr<CScreenBillboard> c_text_billboard;

    // PostFX
    std::shared_ptr<RadialBlur> radial_blur_fx;
    std::shared_ptr<Bloom> bloom_fx;
    
    ID3D11ShaderResourceView* fond_menu;
    ID3D11ShaderResourceView* fond_menu_jeux;

    float terrain_heigth_offset;

#ifndef NEDITOR
    std::unique_ptr<Camera> debug_camera;
    std::unique_ptr<DebugCameraController> debug_camera_controller;
    bool is_using_debug_camera_;
    bool is_pressing_debug;
    bool is_pressing_reload;
    bool is_pressing_disable_culling;
    bool disable_cursor_lock;
#endif

    bool is_using_debug_camera() const noexcept;
    
    Entity camera_entity;

    void init_debug_camera();
    bool init_shaders();
    bool init_textures();

    void load_mesh(Assimp::Importer& importer, std::string_view mesh_name, std::string_view mesh_path);
    bool init_meshes();
    bool init_materials();
    bool init_cameras();
    bool init_inputs();
    bool init_terrain();
    bool init_scene();
    bool init_splines();
    bool init_resources();
    bool init_skybox();
    bool init_postFX();

    void reload_scene();

    bool finalize_player_movement();

    ControllerLayout& controller_layout;
    std::shared_ptr<InputController> controller;
    std::unique_ptr<ForwardCommand> fwd;
    std::unique_ptr<BackwardCommand> bck;
    std::unique_ptr<RightCommand> rgt;
    std::unique_ptr<LeftCommand> lft;
    std::unique_ptr<JumpCommand> jump;

	MaterielPtr make_material(const aiMaterial& material);
    MaterielPtr make_and_save_material(const std::string& name, const aiMaterial& material);
    MaterielPtr extract_material_from(const aiMaterial& material);
    const aiScene* load_scene_from_file(Assimp::Importer& importer, const std::string& path);

    void apply_post_fxs();
    void prepare_camera();
    void render_systems();
    void render_terrain();
    void render_statics();

#ifndef NEDITOR
    void render_transforms();
    void render_debug();
#endif

    void update_debug_camera(const PM3D::CDIManipulateur& input, FrameDuration dt);
    void handle_debug_inputs(const PM3D::CDIManipulateur& input, FrameDuration dt);

    void cleanup_physic();
    void cleanup_cameras();
    void cleanup_ecs();
public:
    BaseCameraController* active_camera_controller();

    Game(const EngineContext<CDispositifD3D11,CDIManipulateur>& ctx, PhysicEngine& physic);
    ~Game();

    // Appelé une fois pour initialiser le jeu
    // Il s'agit de la première méthode appelée (à l'exception du constructeur)
    ENGINE_CALLBACK bool init();

    // Appelé lorsque les entrées utilisateurs
    ENGINE_CALLBACK void handle_inputs(const PM3D::CDIManipulateur& inputs, FrameDuration dt);

    // Appelé juste avant la simulation physique
    ENGINE_CALLBACK void pre_physic_update(FrameDuration dt);

    // Appelé juste après la simulation physique
    ENGINE_CALLBACK void post_physic_update(FrameDuration dt);

    // Appelé une fois par frame
    ENGINE_CALLBACK void update(FrameDuration dt);

    // Appelé une fois par frame après toutes les autres mise à jour
    ENGINE_CALLBACK void late_update(FrameDuration dt);

    ENGINE_CALLBACK void start_of_frame(FrameDuration dt);
    ENGINE_CALLBACK void end_of_frame(FrameDuration dt);

    // Appelé juste avant de réaliser l'affichage
    ENGINE_CALLBACK void pre_render();

    // Appelé durant l'affichage (les appels d3d11 se font ici)
    ENGINE_CALLBACK void render();

    // Appelé une fois pour nettoyer le jeu. Aucune autre fonction ne sera appelée
    ENGINE_CALLBACK void cleanup();

    // Appelé lorsque la taille de l'écran change
    ENGINE_CALLBACK void screen_size_changed(int width, int height);

    ENGINE_CALLBACK bool should_quit() const noexcept;

    ENGINE_CALLBACK bool is_paused() const noexcept;
};
}

#endif
