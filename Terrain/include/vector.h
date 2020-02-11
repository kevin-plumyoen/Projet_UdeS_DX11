#ifndef DEF_VECTOR_H
#define DEF_VECTOR_H

#include <cmath>
#include <iostream>

namespace ng {

template<typename ComponentType, typename ScalarType = ComponentType>
struct Vec3 {
    using component_type = ComponentType;
    using scalar_type = ScalarType;

    component_type x;
    component_type y;
    component_type z;

    constexpr Vec3() noexcept : x{}, y{}, z{} {}
    constexpr Vec3(component_type x, component_type y, component_type z) : x{x}, y{y}, z{z} {}

    scalar_type length() const noexcept {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 to_unit() const {
        const float length_ = length();
        
        if (length_ > 0) {
            return Vec3{x / length_, y / length_, z / length_};
        }

        return Vec3{};
    }

    Vec3 cross(const Vec3& other) const noexcept {
        return Vec3{ y * other.z - z * other.y,
                     z * other.x - x * other.z,
                     x * other.y - y * other.x };
    }

    constexpr Vec3 operator+(const Vec3& other) const noexcept {
        return Vec3{x + other.x, y + other.y, z + other.z};
    }

    constexpr Vec3 operator-(const Vec3& other) const noexcept {
        return Vec3{x - other.x, y - other.y, z - other.z};
    }

    constexpr Vec3 operator*(scalar_type scalar) const noexcept {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }
};

template<typename ComponentType, typename ScalarType>
std::ostream& operator<<(std::ostream& os, const Vec3<ComponentType, ScalarType>& vec3) noexcept {
    os << vec3.x << vec3.y << vec3.z;
}

using Vec3f = Vec3<float>;

}

#endif