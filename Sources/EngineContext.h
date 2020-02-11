#ifndef DEF_ENGINE_CONTEXT_H
#define DEF_ENGINE_CONTEXT_H

#include "GestionnaireDeTextures.h"
#include "Gestionnaires.h"

namespace PM3D {

template<typename Dispositif, typename InputHandler>
struct EngineContext {
    Dispositif& dispositif;
    CGestionnaireDeTextures& textures;
    GestionnaireShader& shaders;
    GestionnaireMateriel& materiels;
    InputHandler& inputs;


    EngineContext(Dispositif& dispo, CGestionnaireDeTextures& textures, GestionnaireShader& shaders, GestionnaireMateriel& materiels, InputHandler& inputs) noexcept
    : dispositif{ dispo }
    , textures{ textures }
    , shaders{shaders}
    , materiels{materiels}
    , inputs{inputs} {

    }
};

}

#endif