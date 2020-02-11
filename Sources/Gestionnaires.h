#ifndef DEF_GESTIONNAIRES_RESOURCES
#define DEF_GESTIONNAIRES_RESOURCES

#include "Entity.h"
#include <unordered_map>
#include <memory>

class Shader;
class Materiel;
struct Mesh;
struct CameraLen;
class InputController;
class PostFX;
class CatmullRom3D;

using ShaderPtr = std::shared_ptr<Shader>;
using MaterielPtr = std::shared_ptr<Materiel>;
using MeshPtr = std::shared_ptr<Mesh>;
using CameraLenPtr = std::shared_ptr<CameraLen>;
using InputControllerPtr = std::shared_ptr<InputController>;
using PostFXPtr = std::shared_ptr<PostFX>;

// Gestionnaire de shaders
using GestionnaireShader = std::unordered_map<std::string, ShaderPtr>;

// Gestionnaire de materiel
using GestionnaireMateriel = std::unordered_map<std::string, MaterielPtr>;

// Gestionnaire de PostFX
using GestionnairePostFX = std::unordered_map<std::string, PostFXPtr>;

using GestionnaireMesh = std::unordered_map<std::string, std::vector<MeshPtr>>;
using GestionnaireCameraLen = std::unordered_map<std::string, CameraLenPtr>;
using GestionnaireInputController = std::unordered_map<std::string, InputControllerPtr>;
using GestionnaireMaterielImporte = std::unordered_map<std::string, std::vector<MaterielPtr>>;

using TagBits = uint32_t;
using GestionnaireTag = std::unordered_map<Entity, TagBits, EntityHash>;

using GestionnaireSpline = std::unordered_map<std::string, CatmullRom3D>;

template<typename T>
struct resource_manager_traits;

template<typename T>
struct is_resource_manager {
    static const bool value = false;
};

template<>
struct is_resource_manager<GestionnaireMateriel> {
    static const bool value = true;
};

template<>
struct resource_manager_traits<GestionnaireMateriel> {
    using resource_type = MaterielPtr;
    using id_type = std::string;
};

template<>
struct is_resource_manager<GestionnaireShader> {
    static const bool value = true;
};

template<>
struct resource_manager_traits<GestionnaireShader> {
    using resource_type = ShaderPtr;
    using id_type = std::string;
};

template<typename Manager, 
         typename = std::enable_if_t<is_resource_manager<Manager>::value>>
bool contains_resource(Manager& manager, typename resource_manager_traits<Manager>::id_type identifier) {
    auto it = manager.find(identifier);

    return it != manager.end();
}

#endif