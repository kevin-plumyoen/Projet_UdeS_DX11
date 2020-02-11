#ifndef DEF_DRAW_CONTEXT_H
#define DEF_DRAW_CONTEXT_H

#include "MathHelpers.h"

class Camera;

namespace PM3D {

struct DrawContext {
    const Camera& active_camera;
    const Mat4x4& view_projection;

    DrawContext(const Camera& cam, const Mat4x4& view_proj) noexcept
    : active_camera(cam)
    , view_projection{view_proj} {

    }
};

}

#endif