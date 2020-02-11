#include "CatmullRom.h"

#include <algorithm>

CatmullRom& CatmullRom::add(Vec2 point) {
    auto it = std::lower_bound(points.begin(), points.end(), point, [](const Vec2& a, const Vec2& b) {
        return a.x < b.x;
    });

    points.insert(it, std::move(point));

    return *this;
}

CatmullRom& CatmullRom::add(float x, float y) {
    return add(Vec2{x, y});
}

std::optional<float> CatmullRom::get(float t) const noexcept {
    assert(points.size() >= 4);
    const auto last_it = std::prev(points.end());

    auto it = std::lower_bound(std::next(points.begin()), std::prev(points.end(), 3), t, [](const Vec2& x, float t) {
        return x.x < t;
    });

    if (it != last_it) {
        const Vec2 p0 = *std::prev(it);
        const Vec2 p1 = *it;
        const Vec2 p2 = *std::next(it);
        const Vec2 p3 = *std::next(it, 2);
        
        //https://www.mvps.org/directx/articles/catmull/
        return 0.5f * ( (2.f * p1.y) + 
                        (-p0.y + p2.y) * t + 
                        (2.f * p0.y - 5.f * p1.y + 4.f * p2.y - p3.y) * (t * t) +
                        (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * (t * t * t));
    }

    return {};
}

CatmullRom3D& CatmullRom3D::add(const Vec3& point) {
    points.push_back(point);
    return *this;
}

CatmullRom3D& CatmullRom3D::add(float x, float y, float z) {
    return add(Vec3{x, y, z});
}

Vec3 CatmullRom3D::get(float t) const noexcept {
    assert(points.size() >= 4);

    const std::size_t start_point_index = static_cast<std::size_t>(t) + 1;
    const std::size_t end_point_index = start_point_index + 1;
    const float float_rest = static_cast<float>(t) - (start_point_index-1);

    assert(start_point_index > 0);
    assert(end_point_index < points.size() - 1);

    const Vec3 p0 = points[start_point_index - 1];
    const Vec3 p1 = points[start_point_index];
    const Vec3 p2 = points[end_point_index];
    const Vec3 p3 = points[end_point_index + 1];

    const DirectX::XMVECTOR p0_ = DirectX::XMLoadFloat3(&p0);
    const DirectX::XMVECTOR p1_ = DirectX::XMLoadFloat3(&p1);
    const DirectX::XMVECTOR p2_ = DirectX::XMLoadFloat3(&p2);
    const DirectX::XMVECTOR p3_ = DirectX::XMLoadFloat3(&p3);

    const DirectX::XMVECTOR res_ = 0.5f * ( ( 2.f * p1_) 
                                          + (-p0_ + p2_) * float_rest
                                          + (2 * p0_ - 5 * p1_ + 4 * p2_ - p3_) * (float_rest * float_rest)
                                          + (-p0_ + 3 * p1_ - 3 * p2_ + p3_) * (float_rest * float_rest * float_rest));

    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, res_);

    return res;
}

void CatmullRom3D::update_points(const Transform& transform)
{
	for (Vec3& point : points) {
		point = transform.transform_point(point);
	}
}

float CatmullRom3D::length() const noexcept {
    assert(points.size() >= 4);
    return static_cast<float>(points.size() - 3);
}