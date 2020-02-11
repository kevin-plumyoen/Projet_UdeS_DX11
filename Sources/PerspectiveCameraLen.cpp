#include "PerspectiveCameraLen.h"
#include "Camera.h"

#include <DirectXMath.h>

PerspectiveCameraLen::PerspectiveCameraLen(float fovy, float aspect_ratio, float near, float far) 
: fovy_{fovy}
, aspect_ratio_{aspect_ratio}
, near_{near}
, far_{far}
, dirty{true} {

}

PerspectiveCameraLen& PerspectiveCameraLen::set_fovy(float fovy) {
    fovy_ = fovy;
    this->dirty = true;

    return *this;
}

PerspectiveCameraLen& PerspectiveCameraLen::set_aspect_ratio(float ratio) {
    aspect_ratio_ = ratio;
    this->dirty = true;

    return *this;
}

PerspectiveCameraLen& PerspectiveCameraLen::set_near(float near) {
    near_ = near;
    this->dirty = true;

    return *this;
}

PerspectiveCameraLen& PerspectiveCameraLen::set_far(float far) {
    far_ = far;
    this->dirty = true;

    return *this;
}

PerspectiveCameraLen& PerspectiveCameraLen::set_planes(float near, float far) {
    return set_near(near).set_far(far);
}

Mat4x4 PerspectiveCameraLen::projection() const {
    if (dirty) {
        projection_ = DirectX::XMMatrixPerspectiveFovLH(fovy(), aspect_ratio(), near(), far());
        dirty = false;
    }

    return projection_;
}

Frustum PerspectiveCameraLen::frustum(const Camera& cam) const {
    return Frustum(cam, near_, far_, fovy_, aspect_ratio_);
}