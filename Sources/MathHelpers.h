#ifndef DEF_CORE_MATH_H
#define DEF_CORE_MATH_H

#include <DirectXMath.h>

using Mat4x4 = DirectX::XMMATRIX;
using Mat3x3 = DirectX::XMFLOAT3X3;

using Vec2 = DirectX::XMFLOAT2;
using Vec3 = DirectX::XMFLOAT3;
using Vec4 = DirectX::XMVECTOR;
using Color = DirectX::XMFLOAT4;
using Quaternion = DirectX::XMVECTOR;

const Vec3 ForwardVector{1.f, 0.f, 0.f};
const Vec3 RightVector{0.f, 1.f, 0.f};
const Vec3 UpVector{0.f, 0.f, 1.f};

namespace math {

Quaternion from_to_rotation(const Vec3& from_direction, const Vec3& to_direction);
Quaternion from_to_rotation(const DirectX::XMVECTOR& from_direction, const DirectX::XMVECTOR& to_direction);

Quaternion look_rotation(const Vec3& forward, const Vec3& up);
Quaternion look_rotation(const DirectX::XMVECTOR& forward, const DirectX::XMVECTOR& up);

}

#endif