#ifndef DEF_CAMERA_LEN_H
#define DEF_CAMERA_LEN_H

#include "MathHelpers.h"
#include "Frustum.h"

class Camera;

struct CameraLen {
    virtual ~CameraLen() = default;

    // Retourne la matrice de projection générée par cette lentille
    virtual Mat4x4 projection() const = 0;
    virtual Frustum frustum(const Camera&) const = 0;
};

#endif