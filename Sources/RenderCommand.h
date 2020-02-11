#ifndef DEF_RENDER_COMMAND_H
#define DEF_RENDER_COMMAND_H

#include "MathHelpers.h"

class Materiel;
class MaterielTerrain;
class CScreenBillboard;

namespace PM3D {
    class CObjet3D;
    class CTexture;
}

struct ID3D11ShaderResourceView;

struct RenderHudObjectCommand {
    CScreenBillboard* billboard;
    ID3D11ShaderResourceView* texture_resource_view;
    int16_t sort_index;

    RenderHudObjectCommand(CScreenBillboard* billboard, ID3D11ShaderResourceView* texture) noexcept
    : RenderHudObjectCommand(billboard, texture, 0) {

    }

    RenderHudObjectCommand(CScreenBillboard* billboard, ID3D11ShaderResourceView* texture, int16_t sort_index) noexcept
    : billboard{billboard}
    , texture_resource_view{texture}
    , sort_index{sort_index} {

    }
};

struct RenderDebugGizmoCommand {
    Mat4x4 model_transform;

    explicit RenderDebugGizmoCommand(Mat4x4 transform)
    : model_transform{ transform } {

    }
};

struct RenderObject3DCommand {
    PM3D::CObjet3D* object;
    Materiel* material;
    Mat4x4 model_transform;
    int16_t sort_index;

    RenderObject3DCommand(PM3D::CObjet3D* object, Materiel* material, Mat4x4 model_transform) noexcept
    : RenderObject3DCommand(object, material, model_transform, 0) {

    }

    RenderObject3DCommand(PM3D::CObjet3D* object, Materiel* material, Mat4x4 model_transform, int16_t sort) noexcept
    : object{ object }
    , material{ material }
    , model_transform{ model_transform } 
    , sort_index{sort} {

    }
};

struct RenderTerrainCommand : public RenderObject3DCommand {
  MaterielTerrain* materiel_terrain;

  RenderTerrainCommand(PM3D::CObjet3D* object, MaterielTerrain* material, Mat4x4 model_transform) noexcept
    : RenderTerrainCommand(object, material, model_transform, 0){

  }

  RenderTerrainCommand(PM3D::CObjet3D* object, MaterielTerrain* material, Mat4x4 model_transform, int16_t sort) noexcept
    : RenderObject3DCommand(object, nullptr, model_transform, sort)
    , materiel_terrain{ material } {

  }
};

#endif