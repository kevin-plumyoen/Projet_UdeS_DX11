#ifndef DEF_REQUIRED_ECS_SYSTEMS
#define DEF_REQUIRED_ECS_SYSTEMS

#include "Gestionnaires.h"

namespace PM3D {
    class CDispositifD3D11;
    class CGestionnaireDeTextures;
    class CDIManipulateur;
}

class PhysicEngine;
struct Renderer;
class TransformHierarchy;

struct RequiredSystems {
    PM3D::CDispositifD3D11& dispositif;
    PM3D::CGestionnaireDeTextures& textures;
    GestionnaireShader& shaders;
    GestionnaireMateriel& materials;
    GestionnaireMesh& meshes;
    GestionnaireCameraLen& camera_lens;
    GestionnaireInputController& input_controllers;
    PhysicEngine& physic;
    PM3D::CDIManipulateur& inputs;
    Renderer& renderer;
    TransformHierarchy& transforms_hierarchy;
    GestionnairePostFX& postFXs;
    GestionnaireMaterielImporte& imported_materials;
    GestionnaireTag& tags;
    GestionnaireSpline& splines;

    RequiredSystems(PM3D::CDispositifD3D11& dispositif, PM3D::CGestionnaireDeTextures& textures, GestionnaireShader& shaders, GestionnaireMateriel& materials, GestionnaireMesh& meshes, GestionnaireCameraLen& camera_lens, GestionnaireInputController& input_controllers, PhysicEngine& physic, PM3D::CDIManipulateur& inputs, Renderer& renderer, TransformHierarchy& hierarchy, GestionnairePostFX& postFXs, GestionnaireMaterielImporte& imported_materials, GestionnaireTag& tags, GestionnaireSpline& splines);
};

#endif