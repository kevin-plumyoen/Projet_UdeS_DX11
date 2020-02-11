#include "Game.h"

#pragma warning(push, 0)  
#include <Gdiplus.h>
#pragma warning(pop)

#include "RequiredSystems.h"

#include "Logger.h"
#include "Profiler.h"

#ifndef NEDITOR
#include "CheatDatabase.h"
#endif

#include "EventBus.h"
#include "EngineEvents.h"

// Rendering
#include "terrain_patch.h"
#include "TerrainPatchEffet.h"
#include "CMeshImporter.h"
#include "DrawContext.h"
#include "dispositifD3D11.h"
#include "D3D11Renderer.h"
#include "PerspectiveCameraLen.h"
#include "Skybox.h"

#include "MaterielTerrain.h"
#include "ShaderTerrain.h"

// Physic
#include <PxPhysicsAPI.h>
#include "PhysicEngine.h"
#include "CollisionMasks.h"
#include "PhysicHelpers.h"

// ECS
#include "UpdateRigidBodyTransformsSystem.h"
#include "StaticMeshRenderingSystem.h"

// Input
#include "DIManipulateur.h"
#include "KeyboardMovementController.h"
#include "ForwardCommand.h"
#include "BackwardCommand.h"
#include "LeftCommand.h"
#include "RightCommand.h"

// Resources
#include "GestionnaireDeTextures.h"

#include "CameraController.h"

// Scene loading
#include "LibraryLoader.h"
#include "SceneLoaderContext.h"
#include "EntityFactoryAPI.h"

// PhysX
#include <PxPhysicsAPI.h>

//HUD
#include "HUD_Manager.h"
#include "Main_Menu.h"
#include "HUD_En_Jeux.h"
#include "In_Game_Menu.h"
#include "Scoreboard_Menu.h"
#include "Option_Menu.h"
#include "Endgame_Menu.h"

// Gameplay
#include "GameplayCameraManager.h"
#include "RadialBlur.h"
#include "OptionDatabase.h"

//score
#include "Score.h"

// Assimp
#undef min
#undef max
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include <cassert>

namespace PM3D {
    Game::Game(const EngineContext<CDispositifD3D11, CDIManipulateur>& ctx, PhysicEngine& physic)
    : dispositif{ ctx.dispositif }
    , textures{ ctx.textures }
    , shaders{ ctx.shaders }
    , materiels{ ctx.materiels }
    , inputs{ ctx.inputs }
    , meshes{}
    , camera_lens{}
    , input_controllers{}
    , font{std::make_unique<Font>(L"Arial", 16.f, Font::Style::Bold) }
    , pAfficheurTexte{ std::make_unique<CAfficheurTexte>(&dispositif, 100, 100, FontContext::get()) }
    , renderer{ std::make_unique<D3D11Renderer>(dispositif) }
    , physic{ physic }
    //, current_score(std::make_unique<CScore>(std::string("score.txt")))
    , scene{ Scene(&entities) }
    , transforms_hierarchy{entities, components.transforms}
    , systems{ components, Systems::RequiredValues(*renderer, physic, transforms_hierarchy), scene }
    , hud(std::make_unique<CHUD_Manager>())
    , entity_factory{ entities, components, RequiredSystems{dispositif, textures, shaders, materiels, meshes, camera_lens, input_controllers, physic, inputs, *renderer, transforms_hierarchy, postFXs,imported_materials, tags, splines},hud.get() }
    , controller_layout{ControllerLayout::get()}
    , quitter{false}
    , paused{true}
    , transmit_pause_changed{true}
    , fond_menu{ textures.GetNewTexture(L"fon_men_princ.dds", &dispositif)->GetD3DTexture() }
    , fond_menu_jeux{ textures.GetNewTexture(L"tron_men.dds", &dispositif)->GetD3DTexture() }
    {
        hud->changeCurrentMenu(std::make_unique<CMain_Menu>(font.get(), &dispositif, renderer.get(),fond_menu));
        EventBus::instance().transmit(PauseStateChangedEvent{true});
    }

    Game::~Game() = default;

    void Game::reload_scene() {
#ifndef NEDITOR
        PROFILE_INSTANT_GLOBAL("reload scene", "editor");
        // Reload scene
        cleanup();
        if(!init_scene()) {
            throw std::runtime_error{"Failed to initialize scene"};
        }
#endif
    }

    void Game::cleanup_physic() {
        for(auto& pair : components.rigidbodies) {
            physic.destroyActor(pair.second.rigidbody);
            pair.second.rigidbody = nullptr;
        }
    }

    void Game::cleanup_cameras() {
        for (const auto& pair : components.cameras) {
            renderer->cameras().destroy(pair.second.handle());
        }
        renderer->cameras().reset();
    }

    void Game::cleanup_ecs() {
        systems.cleanup();
        components.cleanup();
        entities.clear();
    }

    void Game::cleanup() {
        PROFILE_SCOPE("cleanup", "game");
        cleanup_physic();
        cleanup_cameras();
        cleanup_ecs();
    }

    void Game::screen_size_changed(int w, int h) {
        PROFILE_SCOPE("screen_size_changed", "game");
        bloom_fx->on_resize(static_cast<unsigned int>(w), static_cast<unsigned int>(h));

        renderer->on_screen_sized_changed(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    }

    bool Game::should_quit() const noexcept {
        return quitter;
    }

    bool Game::is_paused() const noexcept {
        return paused;
    }

    #pragma region Initialisation
    bool Game::init() {
        PROFILE_SCOPE("init", "game");
        return init_resources()
            && init_terrain()
            && init_skybox()
            && init_scene();
    }

    bool Game::init_terrain() {
        PROFILE_SCOPE("init_terrain", "game");

        std::vector<ng::TerrainPatch> patches;

        float min_height = std::numeric_limits<float>::max();
        float max_height = std::numeric_limits<float>::min();

        for(int i = 0; i < 4; ++i) {
            log_info("game.init_terrain", "loading terrain patch " + std::to_string(i));

            std::ifstream patch_file(std::to_string(i) + ".patch", std::ios::binary);
            if(!patch_file) {
                log_critical("game.init_terrain", "failed to open patch file");
                return false;
            }

            ng::TerrainPatch patch;
            if(!patch.load(patch_file)) {
                log_critical("game.init_terrain", "failed to load patch file ");
                return false;
            }

            const std::vector<float> heights = patch.getHeights();
            min_height = std::min(min_height, *std::min_element(heights.begin(), heights.end()));
            max_height = std::max(max_height, *std::max_element(heights.begin(), heights.end()));

            patches.push_back(std::move(patch));
        }

        const float diff_height = max_height - min_height;
        const float half_diff_height = diff_height / 2.f;

        terrain_heigth_offset = -half_diff_height;

        for(const ng::TerrainPatch& patch : patches) {
            terrain_patches.push_back(std::make_unique<CTerrainPatchEffet>(patch, &dispositif));
        }

        std::vector<float> heights;
        patch_width = patches.front().width();
        patch_depth = patches.front().depth();
        tile_size = patches.front().tile_size();

        // First two patches
        for(uint32_t i = 0; i < patch_depth; ++i) {
            const std::vector<float> heights_patch_0 = patches[0].getHeights(i);
            const std::vector<float> heights_patch_1 = patches[1].getHeights(i);
            heights.insert(heights.end(), heights_patch_0.begin(), std::prev(heights_patch_0.end()));
            heights.insert(heights.end(), heights_patch_1.begin(), heights_patch_1.end());
        }

        for(uint32_t i = 1; i < patch_depth; ++i) {
            const std::vector<float> heights_patch_2 = patches[2].getHeights(i);
            const std::vector<float> heights_patch_3 = patches[3].getHeights(i);
            heights.insert(heights.end(), heights_patch_2.begin(), std::prev(heights_patch_2.end()));
            heights.insert(heights.end(), heights_patch_3.begin(), heights_patch_3.end());
        }

        std::transform(heights.begin(), heights.end(), heights.begin(), [this](float height) {
            return height + this->terrain_heigth_offset;
        });

        auto heighfield = physic.createHeightField(heights, tile_size, tile_size, (patch_width * 2) - 1,  (patch_depth * 2) - 1);
        PhysicHelper::set_query_filter_mask(heighfield, CollisionMasks::CameraObstacleBit | CollisionMasks::GrappableBit);

        return true;
    }

    #pragma region Resources
    bool Game::init_resources() {
        return init_textures()
            && init_shaders()
            && init_postFX()
            && init_meshes()
            && init_materials()
            && init_cameras()
            && init_inputs()
            && init_splines();
    }

    bool Game::init_splines() {
        log_info("game.init", "initialisation des splines...");

        CatmullRom3D spline;
        spline
            .add(Vec3{0, 0, 0})
            .add(Vec3{0, 60, 20})
            .add(Vec3{0, 40, 30})
            .add(Vec3{0, 100, 25});

        splines["test"] = std::move(spline);

		CatmullRom3D spline_circle;
		spline_circle
			.add(Vec3{ 0, 20, 0 })
			.add(Vec3{ 0, 0, 0 })
			.add(Vec3{ 20, 0, 0 })
			.add(Vec3{ 20, 20, 0 })
			.add(Vec3{ 0, 20, 0 })
			.add(Vec3{ 0, 0, 0 })
			.add(Vec3{ 0, -20, 0 });

		splines["circle"] = std::move(spline_circle);

        return true;
    }

    bool Game::init_shaders() {
        log_info("game.init", "initialisation des shaders...");
        shaders["MiniPhong.fx"] = std::make_shared<Shader>(L"MiniPhong.fx", &dispositif);
        shaders["Skybox.fx"] = std::make_shared<Shader>(L"Skybox.fx", &dispositif);

        terrain_shader = std::make_shared<ShaderTerrain>(L"Terrain.fx", &dispositif);

        return true;
    }

    bool Game::init_textures() {
        textures.GetNewTexture(L"reticule_de_la_mort.dds", &dispositif);

        reticule_billboard = std::make_unique<CScreenBillboard>(Vec2{0.f, 0.f}, Vec2{0.1f, 0.1f}, &dispositif);

        c_text_billboard = std::make_unique<CScreenBillboard>(Vec2{ 0.8f,-0.8f }, Vec2{ 0.1f, 0.2f }, &dispositif);

        return true;
    }

    void Game::load_mesh(Assimp::Importer& importer, std::string_view mesh_name, std::string_view mesh_path) {
        const aiScene* mesh_scene = load_scene_from_file(importer, std::string{ mesh_path });

        std::vector<std::shared_ptr<Mesh>> imported_meshes;
        std::vector<MaterielPtr> materials_of_imported_mesh;

        if (mesh_scene) {
            for (unsigned int mesh_index = 0; mesh_index < mesh_scene->mNumMeshes; ++mesh_index) {
                const aiMesh* mesh = mesh_scene->mMeshes[mesh_index];
                const aiMaterial* mesh_material = mesh_scene->mMaterials[mesh->mMaterialIndex];

                // On extrait la materiel
                MaterielPtr material = extract_material_from(*mesh_material);

                imported_meshes.push_back(std::make_shared<Mesh>(mesh));
                materials_of_imported_mesh.push_back(material);
            }

            meshes[std::string{ mesh_name }] = std::move(imported_meshes);
            imported_materials[std::string{ mesh_name }] = std::move(materials_of_imported_mesh);
        }

    }

    bool Game::init_meshes() {
        Assimp::Importer imp;

        load_mesh(imp, "shitty_roman_house", "shittyromanhouse.obj");
        load_mesh(imp, "cube", "cube.fbx");
        load_mesh(imp, "skybox", "skybox.fbx");
        load_mesh(imp, "building_part", "Building_Part.fbx");
        load_mesh(imp, "building_roof", "building_roof.fbx");
        load_mesh(imp, "chair", "chaise.fbx");
        load_mesh(imp, "table", "table.fbx");
        load_mesh(imp, "fence", "barriere_campagne.fbx");
        load_mesh(imp, "smooth_sphere", "smooth_sphere.obj");
        load_mesh(imp, "wall", "murs/mur.obj");
        load_mesh(imp, "kiki", "Salsa_64.obj");
        load_mesh(imp, "sphere_light", "SphereLight.obj");
        load_mesh(imp, "player", "player.obj");
        load_mesh(imp, "limit", "Map_limits.obj");
        load_mesh(imp, "lampadaire", "lampadaire.obj");

        return true;
    }

    bool Game::init_materials() {
        PROFILE_SCOPE("init_materials", "game");
        log_info("game.init", "initialisation des matériaux...");
        CDispositifD3D11* pDispositif = &dispositif;

        ///////////////////////////////////// Matériels Terrain /////////////////////////////////////////////

        terrain_material_z1 = std::make_shared<MaterielTerrain>();
        terrain_material_z1->shader = terrain_shader;
        terrain_material_z1->ambientColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z1->diffuseColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z1->emissionColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z1->specularColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z1->specularIntensity = 0.3f;
        terrain_material_z1->specularSpread = 4.0f;
        terrain_material_z1->diffuse1Map = textures.GetNewTexture(L"Asphalt_001_COLOR.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->diffuse2Map = textures.GetNewTexture(L"Grass_001_COLOR.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->diffuse3Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->normal1Map = textures.GetNewTexture(L"Asphalt_001_NRM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->normal2Map = textures.GetNewTexture(L"Grass_001_NRM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->normal3Map = textures.GetNewTexture(L"Black_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->emission1Map = textures.GetNewTexture(L"Black_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->emission2Map = textures.GetNewTexture(L"Black_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->emission3Map = textures.GetNewTexture(L"Floor_Green_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z1->layoutMap = textures.GetNewTexture(L"layout1.dds", pDispositif)->GetD3DTexture();

        terrain_material_z2 = std::make_shared<MaterielTerrain>();
        terrain_material_z2->shader = terrain_shader;
        terrain_material_z2->ambientColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z2->diffuseColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z2->emissionColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z2->specularColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z2->specularIntensity = 0.3f;
        terrain_material_z2->specularSpread = 4.0f;
        terrain_material_z2->diffuse1Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z2->diffuse2Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z2->diffuse3Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z2->emission1Map = textures.GetNewTexture(L"Floor_Yellow_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z2->emission2Map = textures.GetNewTexture(L"Floor_White_Trail_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z2->emission3Map = textures.GetNewTexture(L"Floor_Green_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z2->layoutMap = textures.GetNewTexture(L"layout2.dds", pDispositif)->GetD3DTexture();

        terrain_material_z3 = std::make_shared<MaterielTerrain>();
        terrain_material_z3->shader = terrain_shader;
        terrain_material_z3->ambientColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z3->diffuseColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z3->emissionColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z3->specularColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z3->specularIntensity = 0.3f;
        terrain_material_z3->specularSpread = 4.0f;
        terrain_material_z3->diffuse1Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z3->diffuse2Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z3->diffuse3Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z3->emission1Map = textures.GetNewTexture(L"Floor_Purple_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z3->emission2Map = textures.GetNewTexture(L"Floor_White_Trail_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z3->emission3Map = textures.GetNewTexture(L"Floor_Green_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z3->layoutMap = textures.GetNewTexture(L"layout3.dds", pDispositif)->GetD3DTexture();

        terrain_material_z4 = std::make_shared<MaterielTerrain>();
        terrain_material_z4->shader = terrain_shader;
        terrain_material_z4->ambientColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z4->diffuseColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z4->emissionColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z4->specularColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        terrain_material_z4->specularIntensity = 0.3f;
        terrain_material_z4->specularSpread = 4.0f;
        terrain_material_z4->diffuse1Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z4->diffuse2Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z4->diffuse3Map = textures.GetNewTexture(L"Bloc_Diffuse.dds", pDispositif)->GetD3DTexture();
        terrain_material_z4->emission1Map = textures.GetNewTexture(L"Floor_Brown_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z4->emission2Map = textures.GetNewTexture(L"Floor_White_Trail_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z4->emission3Map = textures.GetNewTexture(L"Floor_Green_EM.dds", pDispositif)->GetD3DTexture();
        terrain_material_z4->layoutMap = textures.GetNewTexture(L"layout4.dds", pDispositif)->GetD3DTexture();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<Materiel> player_materiel = std::make_shared<Materiel>();
        player_materiel->shader = shaders["MiniPhong.fx"];
        player_materiel->ambientColor = Couleur(0.7f, 0.3f, 0.3f, 1.0f);
        player_materiel->diffuseColor = Couleur(0.7f, 0.3f, 0.3f, 1.0f);
        player_materiel->emissionColor = Couleur(0.0f, 0.0f, 0.0f, 1.0f);
        player_materiel->specularColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        player_materiel->specularIntensity = 0.7f;
        player_materiel->specularSpread = 16.0f;
        materiels["player"] = player_materiel;

        std::shared_ptr<Materiel> transparent_player_materiel = std::make_shared<Materiel>();
        transparent_player_materiel->shader = shaders["MiniPhong.fx"];
        transparent_player_materiel->ambientColor = Couleur(0.7f, 0.3f, 0.3f, 0.0f);
        transparent_player_materiel->diffuseColor = Couleur(0.7f, 0.3f, 0.3f, 0.6f);
        transparent_player_materiel->emissionColor = Couleur(0.0f, 0.0f, 0.0f, 0.0f);
        transparent_player_materiel->specularColor = Couleur(1.0f, 1.0f, 1.0f, 0.0f);
        transparent_player_materiel->specularIntensity = 0.7f;
        transparent_player_materiel->specularSpread = 16.0f;
        transparent_player_materiel->is_transparent = true;
        materiels["transparent_player"] = transparent_player_materiel;
        
        std::shared_ptr<Materiel> limit_materiel = std::make_shared<Materiel>();
        limit_materiel->shader = shaders["MiniPhong.fx"];
        limit_materiel->ambientColor = Couleur(0.0f, 0.0f, 0.0f, 0.0f);
        limit_materiel->diffuseColor = Couleur(0.0f, 0.0f, 0.0f, 0.0f);
        limit_materiel->emissionColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        limit_materiel->specularColor = Couleur(0.0f, 0.0f, 0.0f, 0.0f);
        limit_materiel->specularIntensity = 1.0f;
        limit_materiel->specularSpread = 1.0f;
        limit_materiel->is_transparent = true;
        limit_materiel->emissionMap = textures.GetNewTexture(L"Map_limits.dds", pDispositif)->GetD3DTexture();
        materiels["limit"] = limit_materiel;
        
        /*std::shared_ptr<Materiel> bois_clair = std::make_shared<Materiel>();
        bois_clair->shader = shaders["MiniPhong.fx"];
        bois_clair->ambientColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bois_clair->diffuseColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bois_clair->emissionColor = Couleur(0.0f, 0.8f, 0.8f, 1.0f);
        bois_clair->specularColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bois_clair->specularIntensity = 0.2f;
        bois_clair->specularSpread = 4.0f;
        bois_clair->diffuseMap = textures.GetNewTexture(L"tex_table.dds", pDispositif)->GetD3DTexture();
        materiels["bois_clair"] = bois_clair;*/
        
        std::shared_ptr<Materiel> bois_fonc = std::make_shared<Materiel>();
        bois_fonc->shader = shaders["MiniPhong.fx"];
        bois_fonc->ambientColor = Couleur(1.f, 1.f, 1.f, 1.0f);
        bois_fonc->diffuseColor = Couleur(1.f, 1.f, 1.f, 1.0f);
        bois_fonc->emissionColor = Couleur(0.0f, 0.0f, 0.0f, 1.0f);
        bois_fonc->specularColor = Couleur(1.f, 1.f, 1., 1.0f);
        bois_fonc->specularIntensity = 0.2f;
        bois_fonc->specularSpread = 4.0f;
        bois_fonc->diffuseMap = textures.GetNewTexture(L"tex_chaise.dds", pDispositif)->GetD3DTexture();
        materiels["bois_fonce"] = bois_fonc;

        std::shared_ptr<Materiel> bloc_grap_z1 = std::make_shared<Materiel>();
        bloc_grap_z1->shader = shaders["MiniPhong.fx"];
        bloc_grap_z1->ambientColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z1->diffuseColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z1->emissionColor = Couleur(0.0f, 0.0f, 0.8f, 1.0f);
        bloc_grap_z1->specularColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z1->emissionMap = textures.GetNewTexture(L"Bloc_Emission.dds", pDispositif)->GetD3DTexture();
        materiels["bloc_grap_z1"] = bloc_grap_z1;
        std::shared_ptr<Materiel> bloc_grap_z2 = std::make_shared<Materiel>();
        bloc_grap_z2->shader = shaders["MiniPhong.fx"];
        bloc_grap_z2->ambientColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z2->diffuseColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z2->emissionColor = Couleur(0.0f, 0.8f, 0.8f, 1.0f);
        bloc_grap_z2->specularColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z2->emissionMap = textures.GetNewTexture(L"Bloc_Emission.dds", pDispositif)->GetD3DTexture();
        materiels["bloc_grap_z2"] = bloc_grap_z2;
        std::shared_ptr<Materiel> bloc_grap_z3 = std::make_shared<Materiel>();
        bloc_grap_z3->shader = shaders["MiniPhong.fx"];
        bloc_grap_z3->ambientColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z3->diffuseColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z3->emissionColor = Couleur(0.3f, 0.0f, 0.8f, 1.0f);
        bloc_grap_z3->specularColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z3->emissionMap = textures.GetNewTexture(L"Bloc_Emission.dds", pDispositif)->GetD3DTexture();
        materiels["bloc_grap_z3"] = bloc_grap_z3;
        std::shared_ptr<Materiel> bloc_grap_z4 = std::make_shared<Materiel>();
        bloc_grap_z4->shader = shaders["MiniPhong.fx"];
        bloc_grap_z4->ambientColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z4->diffuseColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z4->emissionColor = Couleur(0.0f, 0.8f, 0.0f, 1.0f);
        bloc_grap_z4->specularColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_grap_z4->emissionMap = textures.GetNewTexture(L"Bloc_Emission.dds", pDispositif)->GetD3DTexture();
        materiels["bloc_grap_z4"] = bloc_grap_z4;
        std::shared_ptr<Materiel> bloc_nograp = std::make_shared<Materiel>();
        bloc_nograp->shader = shaders["MiniPhong.fx"];
        bloc_nograp->ambientColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_nograp->diffuseColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_nograp->emissionColor = Couleur(0.8f, 0.0f, 0.0f, 1.0f);
        bloc_nograp->specularColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_nograp->emissionMap = textures.GetNewTexture(L"Bloc_Emission.dds", pDispositif)->GetD3DTexture();
        materiels["bloc_nograp"] = bloc_nograp;

        std::shared_ptr<Materiel> bloc_color_change = std::make_shared<Materiel>();
        bloc_color_change->shader = shaders["MiniPhong.fx"];
        bloc_color_change->ambientColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_color_change->diffuseColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_color_change->emissionColor = Couleur(0.0f, 0.8f, 0.8f, 1.0f);
        bloc_color_change->specularColor = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
        bloc_color_change->emissionMap = textures.GetNewTexture(L"Bloc_Emission.dds", pDispositif)->GetD3DTexture();
        materiels["bloc_color_change"] = bloc_color_change;

        std::shared_ptr<Materiel> building_materiel = std::make_shared<Materiel>();
        building_materiel->shader = shaders["MiniPhong.fx"];
        building_materiel->ambientColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        building_materiel->diffuseColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        building_materiel->emissionColor = Couleur(0.0f, 0.0f, 0.0f, 1.0f);
        building_materiel->specularColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        building_materiel->specularIntensity = 0.2f;
        building_materiel->specularSpread = 4.0f;
        building_materiel->diffuseMap = textures.GetNewTexture(L"Wall_Test_Texture.dds", pDispositif)->GetD3DTexture();
        building_materiel->normalMap = textures.GetNewTexture(L"Wall_Test_Normal.dds", pDispositif)->GetD3DTexture();
        materiels["building"] = building_materiel;

        std::shared_ptr<Materiel> building_roof_materiel = std::make_shared<Materiel>();
        building_roof_materiel->shader = shaders["MiniPhong.fx"];
        building_roof_materiel->ambientColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        building_roof_materiel->diffuseColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        building_roof_materiel->emissionColor = Couleur(0.0f, 0.0f, 0.0f, 1.0f);
        building_roof_materiel->specularColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        building_roof_materiel->specularIntensity = 0.2f;
        building_roof_materiel->specularSpread = 4.0f;
        building_roof_materiel->diffuseMap = textures.GetNewTexture(L"Asphalt_001_COLOR.dds", pDispositif)->GetD3DTexture();
        building_roof_materiel->normalMap = textures.GetNewTexture(L"Asphalt_001_NRM.dds", pDispositif)->GetD3DTexture();
        materiels["building_roof"] = building_roof_materiel;
        
        std::shared_ptr<Materiel> tree_material = std::make_shared<Materiel>();
        tree_material->shader = shaders["MiniPhong.fx"];
        tree_material->ambientColor = Couleur( 1.0f, 1.0f, 1.0f, 0.f);
        tree_material->diffuseColor = Couleur( 1.0f, 1.0f, 1.0f, 1.f);
        tree_material->specularColor = Couleur(0.0f, 0.0f, 0.0f, 0.f);
        tree_material->emissionColor = Couleur(0.1f, 0.1f, 0.1f, 0.f);
        tree_material->is_transparent = true;
        tree_material->diffuseMap = textures.GetNewTexture(L"arbre_1.dds", pDispositif)->GetD3DTexture();
        materiels["tree_material"] = tree_material;

        std::shared_ptr<Materiel> npc_material = std::make_shared<Materiel>();
        npc_material->shader = shaders["MiniPhong.fx"];
        npc_material->ambientColor = Couleur(1.0f, 1.0f, 1.0f, 0.f);
        npc_material->diffuseColor = Couleur(1.0f, 1.0f, 1.0f, 1.f);
        npc_material->specularColor = Couleur(0.0f, 0.0f, 0.0f, 0.f);
        npc_material->emissionColor = Couleur(0.1f, 0.1f, 0.1f, 0.f);
        npc_material->is_transparent = true;
        npc_material->diffuseMap = textures.GetNewTexture(L"NPC.dds", pDispositif)->GetD3DTexture();
        materiels["npc_material"] = npc_material;

		std::shared_ptr<Materiel> pilier = std::make_shared<Materiel>();
		pilier->shader = shaders["MiniPhong.fx"];
		pilier->ambientColor = Couleur(1.0f, 1.0f, 1.0f, 1.f);
		pilier->diffuseColor = Couleur(0.064176f, 0.640000f, 0.233417f, 1.f);
		pilier->specularColor = Couleur(0.0f, 0.0f, 0.0f, 0.f);
		pilier->emissionColor = Couleur(0.080220f, 0.800000f, 0.291771f,0.8f);
		pilier->diffuseMap = textures.GetNewTexture(L"ligne_blanche.dds", pDispositif)->GetD3DTexture();
		materiels["pilier_materiel"] = pilier;

        skybox_material = std::make_shared<Materiel>();
        skybox_material->shader = shaders["Skybox.fx"];
        skybox_material->emissionColor = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
        skybox_material->diffuseMap = textures.GetNewTexture(L"high_res_tron_skybox.dds", pDispositif)->GetD3DTexture();
        materiels["skybox"] = skybox_material;

        materiels["grappling_cable"] = materiels["Opaque_Core"];

        std::shared_ptr<Materiel> transparent_material = std::make_shared<Materiel>();
        transparent_material->is_visible = false;
        materiels["transparent"] = transparent_material;

        std::shared_ptr<Materiel> transparent_core = std::make_shared<Materiel>(*materiels["Opaque_Core"]);
        transparent_core->is_transparent = true;
        transparent_core->diffuseColor.a(0.0f);
        transparent_core->ambientColor.a(0.4f);
        transparent_core->emissionColor.a(0.0f);
        transparent_core->specularColor.a(0.0f);
        materiels["Transparent_Core"] = transparent_core;

        std::shared_ptr<Materiel> transparent_frame = std::make_shared<Materiel>(*materiels["Opaque_Frame"]);
        transparent_frame->is_transparent = true;
        transparent_frame->diffuseColor.a(0.0f);
        transparent_frame->ambientColor.a(0.6f);
        transparent_frame->emissionColor.a(0.0f);
        transparent_frame->specularColor.a(0.0f);
        materiels["Transparent_Frame"] = transparent_frame;

        return true;
    }

    bool Game::init_postFX() {
        radial_blur_fx = std::make_shared<RadialBlur>(L"RadialBlur.fx", &dispositif);
        //radial_blur_fx->setVitesse(10.f);

        postFXs["radial_blur"] = radial_blur_fx;
        postFXs["brightness"] = std::make_shared<PostFX>(L"Brightness.fx", &dispositif);

        bloom_fx = std::make_shared<Bloom>(L"Bloom.fx", &dispositif);
        postFXs["bloom"] = bloom_fx;

        return true;
    }

    #pragma endregion

    #pragma region Scene
    bool Game::init_skybox() {
        skybox_object = std::make_unique<Skybox>(*meshes.at("skybox").front(), &dispositif);

        return true;
    }

    bool Game::finalize_player_movement() {
        if(!camera_entity.valid()) return true;

        auto get_active_camera = [this]() {
            return active_camera_controller();
        };

        fwd = std::make_unique<ForwardCommand>(get_active_camera);
        bck = std::make_unique<BackwardCommand>(get_active_camera);
        rgt = std::make_unique<RightCommand>(get_active_camera);
        lft = std::make_unique<LeftCommand>(get_active_camera);
        jump = std::make_unique<JumpCommand>(physic);

        std::shared_ptr<KeyboardMovementController> keyboard_movement_controller = std::dynamic_pointer_cast<KeyboardMovementController>(input_controllers["player_movement"]);
        keyboard_movement_controller->set_inputs(fwd.get(), bck.get(), rgt.get(), lft.get(), jump.get());

        return true;
    }

    bool Game::init_scene() {
        using load_scene_fn = int(*)(void*);

        // Prepare scene loader context
        SceneLoaderContext ctx(&entity_factory);
        ctx.scene_api.add_entity_in_zone = api::add_entity_in_zone;
        ctx.scene_api.scene = &scene;

        // Load scene
        LibraryLoader scene_loader_lib(L"Scene.dll");
        load_scene_fn scene_loader = scene_loader_lib.symbol<load_scene_fn>("load_scene");

        if(scene_loader(&ctx) != 0) {
            log_critical("game.init_scene", "failed to load scene");
            return false;
        }

        camera_entity = ctx.camera_entity;

        // yerk, on finit l'initialisation du controlleur de mouvement
        if(!finalize_player_movement()) {
            log_critical("game.init_scene", "failed to finalize player movement controller");
            return false;
        }
       
        return true;
    }

    bool Game::init_cameras() {
        using namespace DirectX;

        log_info("game.init", "initialisation des caméras...");


        // Initialisation des objets 3D - création et/ou chargement
        const float champDeVision = XM_PI / 4.f; 	// 45 degrés
        const float ratioDAspect = static_cast<float>(dispositif.GetLargeur()) / static_cast<float>(dispositif.GetHauteur());
        const float planRapproche = 0.1f;
        const float planEloigne = 1000.0f;

        camera_lens["main_camera"] = std::make_shared<PerspectiveCameraLen>(champDeVision, ratioDAspect, planRapproche, planEloigne);

        init_debug_camera();

        return true;
    }

    void Game::init_debug_camera() {
    #ifndef NEDITOR
        debug_camera = std::make_unique<Camera>(camera_lens["main_camera"], Vec3{0.f, 0.f, 0.f}, Vec3{0.f, 1.f, 0.f});
        debug_camera_controller = std::make_unique<DebugCameraController>(*debug_camera);
        
        is_pressing_debug = false;
        is_using_debug_camera_ = false;
        disable_cursor_lock = false;
    #endif
    }

    bool Game::init_inputs() {
        PROFILE_SCOPE("init_inputs", "game");
        log_info("game.init", "initialisation des entrées utilisateurs...");

        controller_layout.up = KeyboardKey::w;
        controller_layout.down = KeyboardKey::s;
        controller_layout.right = KeyboardKey::d;
        controller_layout.left = KeyboardKey::a;
        controller_layout.jump = KeyboardKey::space;
        controller_layout.change_camera = KeyboardKey::f2;

        auto movement_controller = std::make_shared<KeyboardMovementController>(&inputs);
        input_controllers["player_movement"] = movement_controller;

        return true;
    }
    #pragma endregion
    #pragma endregion

    #pragma region Material handling
    static std::string to_string(const aiString& string);

    MaterielPtr Game::make_material(const aiMaterial& material) {
        MaterielPtr new_material = std::make_shared<Materiel>();
        new_material->shader = shaders["MiniPhong.fx"]; // TODO: Not hardcode it
        new_material->load(material, textures, &dispositif);

        return new_material;
    }

    MaterielPtr Game::make_and_save_material(const std::string& name, const aiMaterial& material) {
        MaterielPtr new_material = make_material(material);

        materiels[name] = new_material;

        return new_material;
    }

    MaterielPtr Game::extract_material_from(const aiMaterial& material) {
        aiString ai_material_name;
        MaterielPtr material_ptr;
        if (material.Get(AI_MATKEY_NAME, ai_material_name) == AI_SUCCESS) {
            const std::string material_name = to_string(ai_material_name);

            auto it = materiels.find(material_name);
            if (it != materiels.end()) {
                material_ptr = it->second;
            }
            else {
                material_ptr = make_and_save_material(material_name, material);
            }
        }
        else {
            material_ptr = make_material(material);
        }

        return material_ptr;
    }
    #pragma endregion

    #pragma region Assimp Helpers
    static std::string to_string(const aiString& string) {
        std::string new_string;
        new_string.reserve(string.length);
        std::copy(string.data, string.data + string.length,
            std::back_inserter(new_string));

        return new_string;
    }

    const aiScene* Game::load_scene_from_file(Assimp::Importer& importer, const std::string& path) {
        const aiScene* scene_ = importer.ReadFile(path.c_str(),
            aiProcess_Triangulate
            | aiProcess_MakeLeftHanded
            | aiProcess_FlipUVs
            | aiProcess_ValidateDataStructure
            | aiProcess_FindInvalidData);

        log_critical("game.loadScene", importer.GetErrorString());

        // Charge tous les matériels de la scène
        if (scene_->HasMaterials()) {
            std::for_each(scene_->mMaterials, scene_->mMaterials + scene_->mNumMaterials, [this](const aiMaterial* material) {
                aiString ai_material_name;
                if (material->Get(AI_MATKEY_NAME, ai_material_name) == AI_SUCCESS) {
                    const std::string material_name = to_string(ai_material_name);

                    // Add a new material
                    auto it = materiels.find(material_name);
                    if (it == std::end(materiels)) {
                        make_and_save_material(material_name, *material);
                    }
                }
                else {
                    // Material has no name
                    // it cannot be stored
                }
            });
        }

        return scene_;
    }
    #pragma endregion

    #pragma region Inputs
    bool Game::is_using_debug_camera() const noexcept {
    #ifndef NEDITOR
        return is_using_debug_camera_;
    #else
        return false;
    #endif
    }

#ifndef NEDITOR
    void Game::update_debug_camera(const PM3D::CDIManipulateur& input, FrameDuration dt) {
        debug_camera_controller->handle_input(input, dt);
    }
 #else
    void Game::update_debug_camera(const PM3D::CDIManipulateur&, FrameDuration) {
    }
#endif

    void Game::handle_debug_inputs(const PM3D::CDIManipulateur& input, FrameDuration /*dt*/) {
    #ifndef NEDITOR
        if (input.ToucheAppuyee(KeyboardKey::enter)) {
            debug_camera_controller->reset_to(renderer->cameras().active_camera());
        }

        // Recharge la scène si F5 est appuyé
        if (input.ToucheAppuyee(KeyboardKey::f5) && !is_pressing_reload) {
            reload_scene();
            is_pressing_reload = true;
        }
        else if (!input.ToucheAppuyee(KeyboardKey::f5) && is_pressing_reload) {
            is_pressing_reload = false;
        }

        if (input.ToucheAppuyee(KeyboardKey::f6) && !disable_cursor_lock) {
            inputs.show_cursor();
            inputs.unlock_cursor();

            disable_cursor_lock = true;
        }

        // K toggles culling
        if (input.ToucheAppuyee(KeyboardKey::k) && !is_pressing_disable_culling) {
            CheatDatabase::get().disable_culling = !CheatDatabase::get().disable_culling;
            OptionsDatabase::get().enable_bloom = !OptionsDatabase::get().enable_bloom;

            is_pressing_disable_culling = true;
        }
        else if (!input.ToucheAppuyee(KeyboardKey::k) && is_pressing_disable_culling) {
            is_pressing_disable_culling = false;
        }

        if (input.ToucheAppuyee(KeyboardKey::pause) && !is_pressing_debug) {
            if (is_using_debug_camera_) {
                renderer->cameras().overload_rendering_camera(nullptr);
                is_using_debug_camera_ = false;
            }
            else {
                renderer->cameras().overload_rendering_camera(debug_camera.get());
                is_using_debug_camera_ = true;
            }

            is_pressing_debug = true;
        }
        else if (!input.ToucheAppuyee(KeyboardKey::pause) && is_pressing_debug) {
            is_pressing_debug = false;
        }
    #endif
    }

    BaseCameraController* Game::active_camera_controller() {
        auto update_it = components.updates.find(camera_entity);
        if(update_it != components.updates.end()) {
            const GameplayCameraManager* camera_manager = static_cast<const GameplayCameraManager*>(update_it->second.userdata);
                
            const Entity active_gameplay_camera_entity = camera_manager->active_camera();
            auto active_gameplay_camera_update_it = components.updates.find(active_gameplay_camera_entity);
            if(active_gameplay_camera_update_it != components.updates.end()) {
                BaseCameraController* camera_controller = static_cast<BaseCameraController*>(active_gameplay_camera_update_it->second.userdata);
                return camera_controller;
            }
        }

        return nullptr;
    }

    void Game::handle_inputs(const PM3D::CDIManipulateur& input, FrameDuration dt) {
        PROFILE_SCOPE("handle_inputs", "game");

        if (is_using_debug_camera()) {
            update_debug_camera(input, dt);
        }
        else {
            hud->gerer_input_menu_courrant(input);
            BaseCameraController* camera_controller = active_camera_controller();


            if (camera_controller) {
                camera_controller->handle_inputs(input);
            }

            systems.inputs->process(entities, dt, ProcessState::HandleInputs);
        }

        handle_debug_inputs(input, dt);
    }
    #pragma endregion

    #pragma region Update
    void Game::pre_physic_update(FrameDuration) {
        PROFILE_SCOPE("pre_physic_update", "game");
    }

    void Game::post_physic_update(FrameDuration dt) {
        PROFILE_SCOPE("post_physic_update", "game");
        systems.triggers->process(entities, dt, ProcessState::PostPhysicUpdate);
        systems.update_rigidbody_transforms->process(entities, dt, ProcessState::PostPhysicUpdate);
    }

    void Game::update(FrameDuration dt) {
        PROFILE_SCOPE("update", "game");
        systems.variable_colors->process(entities, dt, ProcessState::Update);
        systems.updates->process(entities, dt, ProcessState::Update);
        systems.follow_spline->process(entities, dt, ProcessState::Update);
    }

    void Game::late_update(FrameDuration dt) {
        PROFILE_SCOPE("late_update", "game");
        systems.updates->process(entities, dt, ProcessState::LateUpdate);
    }

    void Game::start_of_frame(FrameDuration dt) {
        PROFILE_SCOPE("start_of_frame", "game");
        if (transmit_pause_changed) {
            EventBus::instance().transmit(PauseStateChangedEvent{paused});
            transmit_pause_changed = false;
        }

        systems.update_transforms_hiearchicaly->process(entities, dt, ProcessState::StartOfFrame);
    }

    void Game::end_of_frame(FrameDuration) {
        PROFILE_SCOPE("end_of_frame", "game");
        for(HUDEvent current_event: EventBus::instance().events<HUDEvent>())
            switch (current_event.event)
            {
            case HUDEvent::Event::MenuOption:
                paused = true;
                transmit_pause_changed = true;
                hud->changeCurrentMenu(std::make_unique<COption_Menu>(font.get(), &dispositif, renderer.get(), fond_menu_jeux));
                break;
            case HUDEvent::Event::MenuReprendre:
                hud->changeCurrentMenu(std::make_unique<CHUD_En_Jeux>(font.get(), &dispositif, renderer.get()));
                paused = false;
                transmit_pause_changed = true;
                break;
            case HUDEvent::Event::MenuQuitter:
                quitter = true;
                break;
            case HUDEvent::Event::MenuScore:
                //hud->changeCurrentMenu(std::make_unique<CScoreboard_Menu>(font.get(), &dispositif, renderer.get(), fond_menu_jeux));
               /* paused = true;
                transmit_pause_changed = true;*/
                break;
            case HUDEvent::Event::OptionDesactiverRadialBlur:
                OptionsDatabase::get().enable_radial_blur = !OptionsDatabase::get().enable_radial_blur;
                break;
            case HUDEvent::Event::OptionDesactiverBloom:
                OptionsDatabase::get().enable_bloom = !OptionsDatabase::get().enable_bloom;
                break;
            case HUDEvent::Event::MenuAfficher:
                hud->changeCurrentMenu(std::make_unique<CIn_Game_Menu>(font.get(), &dispositif, renderer.get(), fond_menu_jeux));
                paused = true;
                transmit_pause_changed = true;
                break;
            case HUDEvent::Event::MenuPrecedent:
                hud->swap_menus();
                hud->afficher_menu();
                paused = true;
                transmit_pause_changed = true;
                break;
            case HUDEvent::Event::MenuAllerMenu:
                hud->changeCurrentMenu(std::make_unique<CMain_Menu>(font.get(), &dispositif, renderer.get(), fond_menu));
                hud->afficher_menu();
                paused = true;
                transmit_pause_changed = true;
                break;
            case HUDEvent::Event::AfficherMenuFinDeJeux:
                hud->changeCurrentMenu(std::make_unique<CEndgame_Menu>(font.get(), &dispositif, renderer.get(), fond_menu));               
                
                hud->afficher_menu();
                //paused = true;
                //transmit_pause_changed = true;
                break;
            }
    }
    #pragma endregion

    #pragma region Rendering
    void Game::prepare_camera() {
        systems.update_cameras->process(entities, std::chrono::microseconds(0), ProcessState::PreRender);

        if(renderer->cameras().has_active()) {
            scene.activation(renderer->cameras().active_camera().frustum()); // update etat active zones
        }
    }

    void Game::render_systems() {
        systems.render_static_meshes->process(entities, std::chrono::microseconds(0), ProcessState::PreRender);
        systems.update_billboards->process(entities, std::chrono::microseconds(0), ProcessState::PreRender);
        systems.render_billboards->process(entities, std::chrono::microseconds(0), ProcessState::PreRender);
    }

    void Game::render_terrain() {
        // +------+-------+
        // |  1   |   3   |
        // +------+-------+
        // |  0   |   2   |
        // +------+-------+

        renderer->push_terrain_command(RenderTerrainCommand(terrain_patches[0].get(), terrain_material_z1.get(), DirectX::XMMatrixTranslation(0.f,                                             0.f,                                             terrain_heigth_offset)));
        renderer->push_terrain_command(RenderTerrainCommand(terrain_patches[1].get(), terrain_material_z2.get(), DirectX::XMMatrixTranslation(static_cast<float>(patch_width - 1) * tile_size, 0.f,                                             terrain_heigth_offset)));
        renderer->push_terrain_command(RenderTerrainCommand(terrain_patches[2].get(), terrain_material_z3.get(), DirectX::XMMatrixTranslation(0.f,                                             static_cast<float>(patch_depth - 1) * tile_size, terrain_heigth_offset)));
        renderer->push_terrain_command(RenderTerrainCommand(terrain_patches[3].get(), terrain_material_z4.get(), DirectX::XMMatrixTranslation(static_cast<float>(patch_width - 1) * tile_size, static_cast<float>(patch_depth - 1) * tile_size, terrain_heigth_offset)));
    }

    void Game::render_statics() {
        if(!renderer->cameras().has_active()) return;
        const Camera& active_camera = renderer->cameras().active_camera();

        renderer->push_opaque_command(RenderObject3DCommand(skybox_object.get(), 
                                                            skybox_material.get(), 
                                                            DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(1.0f,0.0f,0.0f,0.0f), DirectX::XM_PI) * DirectX::XMMatrixTranslation(active_camera.getPosition().x, active_camera.getPosition().y, active_camera.getPosition().z),
                                                            -1000));

        render_terrain();

        renderer->push_hud_command(RenderHudObjectCommand(reticule_billboard.get(), textures.GetTexture(L"reticule_de_la_mort.dds")->GetD3DTexture()));
        renderer->push_hud_command(RenderHudObjectCommand(c_text_billboard.get(), pAfficheurTexte->GetTextureView()));
    }

    #ifndef NEDITOR
    void Game::render_transforms() {
        for (const auto& transform_pair : components.transforms) {
            renderer->push_debug_gizmo_command(RenderDebugGizmoCommand(transform_pair.second.world.local_to_world()));
        }
    }

    void Game::render_debug() {
        renderer->reset_post_fx();
        render_transforms();
    }
    #endif

    void Game::apply_post_fxs() {
        std::vector<std::shared_ptr<PostFX>> post_fxs_to_apply;

        if (OptionsDatabase::get().enable_bloom) {
            post_fxs_to_apply.push_back(postFXs["bloom"]);
        }

        if (OptionsDatabase::get().enable_radial_blur) {
            post_fxs_to_apply.push_back(postFXs["radial_blur"]);
        }

        renderer->apply_post_fxs(post_fxs_to_apply);
    }

    void Game::pre_render() {
        PROFILE_SCOPE("pre_render", "game");

        apply_post_fxs();

        prepare_camera();
        render_systems();
        render_statics();

        hud->afficher_menu();

#ifndef NEDITOR
        if (is_using_debug_camera()) {
            render_debug();
        }
#endif
    }

    void Game::render() {
        PROFILE_SCOPE("render", "game");
        systems.lights->process(entities, std::chrono::microseconds(0), ProcessState::PostRender);
        renderer->render_frame();
    }
    #pragma endregion
}

