#ifndef DEF_BILLBOARD_H
#define DEF_BILLBOARD_H

#include "objet3d.h"
#include "MathHelpers.h"

namespace PM3D {
class CDispositifD3D11;
}

struct ID3D11Buffer;

class Billboard : public PM3D::CObjet3D {
    PM3D::CDispositifD3D11* dispositif;
    ID3D11Buffer* vertices_buffer;
    PM3D::HitBox hitbox_;
public:
    Billboard(Vec2 half_extend, PM3D::CDispositifD3D11* pDispositif);
    ~Billboard();

    // Copy is disabled
    Billboard(const Billboard&) = delete;
    Billboard& operator=(const Billboard&) = delete;

    void Draw() override;
    PM3D::HitBox BoundingBox() const noexcept override;
};

#endif