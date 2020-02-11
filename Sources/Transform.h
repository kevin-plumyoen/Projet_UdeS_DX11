#ifndef DEF_TRANSFORM_H
#define DEF_TRANSFORM_H

#include "MathHelpers.h"

struct Transform {
    Quaternion rotation;
    Vec3 translation;
    Vec3 scaling;

    Transform() noexcept;
    Transform(Vec3 translation, Quaternion rotation, Vec3 scaling = Vec3{1.f, 1.f, 1.f});
    Transform(Mat4x4 matrix) noexcept;

    void reset() noexcept;
    Mat4x4 local_to_world() const noexcept;
    Mat4x4 world_to_local() const noexcept;

    Vec3 right() const noexcept;
    Vec3 left() const noexcept;

    Vec3 up() const noexcept;
    Vec3 down() const noexcept;

    Vec3 forward() const noexcept;
    Vec3 backward() const noexcept;

    Vec3 eulerAngles() const noexcept;

    Transform& look_at(const Vec3& target) noexcept;
    Transform& look_at(const Vec3& target, const Vec3& up) noexcept;

    Transform& rotate(float angle, Vec3 axis) noexcept;
    Vec3 scale(Vec3 vector) const noexcept;

    // Transform a local point to world space
    Vec3 transform_point(Vec3 point) const noexcept;

    // Transform a world point to local space
    Vec3 inverse_transform_point(Vec3 point) const noexcept;

    Transform operator*(const Transform& t) const noexcept;
};

#endif