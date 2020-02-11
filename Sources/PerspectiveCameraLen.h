#ifndef DEF_PERSPECTIVE_CAMERA_LEN_H
#define DEF_PERSPECTIVE_CAMERA_LEN_H

#include "CameraLen.h"

#undef near
#undef far

class PerspectiveCameraLen : public CameraLen {
    mutable Mat4x4 projection_;
    mutable bool dirty;

    float fovy_;
    float aspect_ratio_;
    float near_;
    float far_;

public:
    PerspectiveCameraLen(float fovy, float aspect_ratio, float near, float far);

    PerspectiveCameraLen& set_fovy(float fovy);
    PerspectiveCameraLen& set_aspect_ratio(float ratio);
    PerspectiveCameraLen& set_near(float near);
    PerspectiveCameraLen& set_far(float far);
    PerspectiveCameraLen& set_planes(float near, float far);
    Mat4x4 projection() const override;

    float fovy() const noexcept { return fovy_; }
    float aspect_ratio() const noexcept { return aspect_ratio_; }
    float near() const noexcept { return near_; }
    float far() const noexcept { return far_; }
    Frustum frustum(const Camera& camera) const override;
};

#endif