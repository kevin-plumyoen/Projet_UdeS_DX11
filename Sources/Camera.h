#ifndef CAMERA_H
#define CAMERA_H

#include "Clock.h"
#include "MathHelpers.h"
#include "Frustum.h"
#include <memory>

namespace PM3D {
    class CDIManipulateur;
}

struct CameraLen;

struct MissingCameraLen{};

struct CameraRay {
    Vec3 origin;
    Vec3 direction;

    CameraRay(Vec3 origin, Vec3 direction) noexcept
    : origin{ origin }
    , direction{ direction } {

    }
};

class Camera {
public:
    using vector_type = Vec3;
    using matrix_type = Mat4x4;
protected:
    std::shared_ptr<CameraLen> len;
    vector_type position;
    vector_type direction;
    vector_type up;

public:
    Camera(std::shared_ptr<CameraLen> len, const vector_type& position, const vector_type& direction);
    Camera(std::shared_ptr<CameraLen> len, const vector_type& position, const vector_type& direction, const vector_type& up);
    ~Camera();

    void setPosition(const vector_type& position_in);
    void setDirection(const vector_type& direction_in);
    void setUp(const vector_type& up_in);

    matrix_type view() const noexcept;
    matrix_type projection() const noexcept;

    vector_type getPosition() const noexcept;
    vector_type getDirection() const noexcept;
    vector_type getUp() const noexcept;

    Frustum frustum() const noexcept;

    CameraRay ray_from_viewport_coord(const Vec2& coord) const noexcept;
};

#endif