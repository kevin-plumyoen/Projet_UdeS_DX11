#include "MathHelpers.h"
#include "Logger.h"

#include <cmath>

using namespace DirectX;

namespace math {

    Quaternion from_to_rotation(const Vec3& from_direction, const Vec3& to_direction) {
        return from_to_rotation(XMLoadFloat3(&from_direction), XMLoadFloat3(&to_direction));
    }

    Quaternion from_to_rotation(const XMVECTOR& from_direction, const XMVECTOR& to_direction) {
        const XMVECTOR axis_ = XMVector3Cross(from_direction, to_direction);
        const XMVECTOR dot_ = XMVector3Dot(from_direction, to_direction);
        const XMVECTOR from_len_ = XMVector3Length(from_direction);
        const XMVECTOR to_len_ = XMVector3Length(to_direction);

        float dot;
        XMStoreFloat(&dot, dot_);

        float from_len;
        XMStoreFloat(&from_len, from_len_);

        float to_len;
        XMStoreFloat(&to_len, to_len_);

        // If vector are nearly parallels
        if (dot > 0.999999f || dot < -0.999999f) {
            log_info("core.math", "from_to_rotation with parallel vectors");
            return XMQuaternionIdentity();
        }

        Vec3 axis;
        XMStoreFloat3(&axis, axis_);

        XMFLOAT4 quat;
        quat.x = axis.x;
        quat.y = axis.y;
        quat.z = axis.z;
        quat.w = std::sqrt((from_len * from_len) * (to_len * to_len) + dot);

        const XMVECTOR quat_ = XMLoadFloat4(&quat);

        return XMQuaternionNormalize(quat_);
    }

    Quaternion look_rotation(const Vec3& forward, const Vec3& up) {
        return from_to_rotation(XMLoadFloat3(&forward), XMLoadFloat3(&up));
    }

    Quaternion look_rotation(const XMVECTOR& p_forward, const XMVECTOR& p_up) {
        const XMVECTOR forward_ = XMVector3Normalize(p_forward);
        const XMVECTOR right_ = XMVector3Normalize(XMVector3Cross(p_up, forward_));
        const XMVECTOR up_ = XMVector3Cross(forward_, right_);

        Vec3 forward;
        XMStoreFloat3(&forward, forward_);

        Vec3 right;
        XMStoreFloat3(&right, right_);

        Vec3 up;
        XMStoreFloat3(&up, up_);

        const float m00 = forward.x;
        const float m01 = forward.y;
        const float m02 = forward.z;
        const float m10 = right.x;
        const float m11 = right.y;
        const float m12 = right.z;
        const float m20 = up.x;
        const float m21 = up.y;
        const float m22 = up.z;

        const float num8 = (m00 + m11) + m22;

        XMFLOAT4 quaternion;
        if (num8 > 0.f)
        {
            float num = std::sqrt(num8 + 1.f);
            quaternion.w = num * 0.5f;
            num = 0.5f / num;

            quaternion.x = (m12 - m21) * num;
            quaternion.y = (m20 - m02) * num;
            quaternion.z = (m01 - m10) * num;

            return XMLoadFloat4(&quaternion);
        }
        if ((m00 >= m11) && (m00 >= m22))
        {
            const float num7 = std::sqrt(((1.f + m00) - m11) - m22);
            const float num4 = 0.5f / num7;

            quaternion.x = 0.5f * num7;
            quaternion.y = (m01 + m10) * num4;
            quaternion.z = (m02 + m20) * num4;
            quaternion.w = (m12 - m21) * num4;

            return XMLoadFloat4(&quaternion);
        }
        if (m11 > m22)
        {
            const float num6 = std::sqrt(((1.f + m11) - m00) - m22);
            const float num3 = 0.5f / num6;

            quaternion.x = (m10 + m01) * num3;
            quaternion.y = 0.5f * num6;
            quaternion.z = (m21 + m12) * num3;
            quaternion.w = (m20 - m02) * num3;

            return XMLoadFloat4(&quaternion);
        }

        const float num5 = std::sqrt(((1.f + m22) - m00) - m11);
        const float num2 = 0.5f / num5;

        quaternion.x = (m20 + m02) * num2;
        quaternion.y = (m21 + m12) * num2;
        quaternion.z = 0.5f * num5;
        quaternion.w = (m01 - m10) * num2;

        return XMLoadFloat4(&quaternion);
    }

}