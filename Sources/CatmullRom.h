#ifndef DEF_CATMULL_ROM_H
#define DEF_CATMULL_ROM_H

#include "MathHelpers.h"
#include "Transform.h"
#include <vector>
#include <optional>

class CatmullRom {
    std::vector<Vec2> points;
public:
    CatmullRom() = default;
    CatmullRom& add(Vec2 point);
    CatmullRom& add(float x, float y);

    std::optional<float> get(float t) const noexcept;
};

class CatmullRom3D {
    std::vector<Vec3> points;
public:
    CatmullRom3D() = default;
    CatmullRom3D& add(const Vec3& point);
    CatmullRom3D& add(float x, float y, float z);

    Vec3 get(float t) const noexcept;
	void update_points(const Transform& transform);

    float length() const noexcept;
};

#endif