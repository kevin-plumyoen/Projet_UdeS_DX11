#ifndef DEF_GIZMO_RENDERER_H
#define DEF_GIZMO_RENDERER_H

#include "MathHelpers.h"
#include <vector>

namespace PM3D {
    class CDispositifD3D11;
}

struct ID3D11DeviceContext;
struct ID3D11Buffer;

class GizmoRenderer {
    PM3D::CDispositifD3D11* dispositif;
    ID3D11Buffer* pVertexBuffer;
    ID3D11Buffer* pIndexBuffer;

    void init_shape();
public:
    GizmoRenderer(PM3D::CDispositifD3D11* dispo);
    ~GizmoRenderer();

    GizmoRenderer(const GizmoRenderer&) = delete;
    GizmoRenderer& operator=(const GizmoRenderer&) = delete;

    void render(ID3D11DeviceContext* render_ctx, std::vector<Mat4x4>&& transforms);
};

#endif