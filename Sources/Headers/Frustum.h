#ifndef DEF_FRUSTUM_H
#define DEF_FRUSTUM_H

#include "MathHelpers.h"
#include "HitBox.h"

#include <vector>
#include <memory>

class Camera;

namespace PM3D {
    struct HitBox;
}

struct Frustum {
    Vec3 n1 = {}, n2 = {}, n3 = {}, n4 = {}; //points du plan near
    Vec3 f1 = {}, f2 = {}, f3 = {}, f4 = {}; //points du plan far
    Vec2 p1 = {}, p2 = {}, p3 = {}, p4 = {}; // points de l'enveloppe englobante du frustum projeté
    float xmin = {}, xmax = {}, ymin = {}, ymax = {};
    float alpha = {}, near_ = {}, far_ = {}, ratio_ = {}; // angle en radian et distances des plans rapproché et éloigné
    double demi_grand_angle;
    Vec3 camera_direction = {}, camera_up = {}, camera_pos = {};

    Frustum() = default;
    Frustum(const Camera &cam, float near, float far, float angle, float ratio);
    void updateFrustum(const Camera &cam);

    bool contains(const PM3D::HitBox& box) const noexcept;
    bool contains(const Vec3& point) const noexcept;
    bool behind(const Vec3& point) const noexcept;

    Frustum& operator=(const Frustum&) = default;

private:
    void projection();

};

#endif