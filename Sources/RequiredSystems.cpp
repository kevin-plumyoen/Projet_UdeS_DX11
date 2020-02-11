#include <DirectXMath.h>
#include "RequiredSystems.h"

RequiredSystems::RequiredSystems(PM3D::CDispositifD3D11& dispositif, PM3D::CGestionnaireDeTextures& textures, GestionnaireShader& shaders, GestionnaireMateriel& materials, GestionnaireMesh& meshes, GestionnaireCameraLen& camera_lens, GestionnaireInputController& input_controllers, PhysicEngine& physic, PM3D::CDIManipulateur& inputs, Renderer& renderer, TransformHierarchy& hierarchy, GestionnairePostFX& postFXs, GestionnaireMaterielImporte& imported_materials, GestionnaireTag& tags, GestionnaireSpline& splines)
: dispositif{dispositif}
, textures{textures}
, shaders{shaders}
, materials{materials}
, meshes{meshes}
, camera_lens{camera_lens}
, input_controllers{input_controllers}
, physic{physic}
, inputs{inputs}
, renderer{renderer}
, transforms_hierarchy(hierarchy)
, postFXs{postFXs} 
, imported_materials{imported_materials}
, tags{tags}
, splines{splines} {

}