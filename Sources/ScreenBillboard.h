#ifndef DEF_SCREEN_BILLBOARD_H
#define DEF_SCREEN_BILLBOARD_H

#include "objet3d.h"
#include "MathHelpers.h"

struct CScreenBillboardVertex {
    Vec3 position;
    Vec2 uv;

    CScreenBillboardVertex() = default;
    CScreenBillboardVertex(const Vec3& position, const Vec2& uv) 
    : position{position}
    , uv{uv} {
    }
};

namespace PM3D {
    class CDispositifD3D11;
}

struct ID3D11Buffer;

class CScreenBillboard : public PM3D::CObjet3D {
    PM3D::CDispositifD3D11* pDispositif;
    ID3D11Buffer* pVertexBuffer;
    PM3D::HitBox bounding_box_;
public:
    CScreenBillboard(Vec2 center, Vec2 half_extends, PM3D::CDispositifD3D11* pDispositif);
    ~CScreenBillboard();

    void Draw() override;
    PM3D::HitBox BoundingBox() const noexcept override;

    // Copy is disabled
    CScreenBillboard(const CScreenBillboard&) = delete;
    CScreenBillboard& operator=(const CScreenBillboard&) = delete;
};

#endif