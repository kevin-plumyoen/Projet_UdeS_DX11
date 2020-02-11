#include "Transform.h"
#include <cmath>

using namespace DirectX;

Transform::Transform() noexcept 
: rotation{XMQuaternionIdentity()}
, translation{0.f, 0.f, 0.f}
, scaling{1.f, 1.f, 1.f} {

}

Transform::Transform(Vec3 translation, Quaternion rotation, Vec3 scaling) 
: rotation{rotation}
, translation{translation} 
, scaling{scaling} {

}

Transform::Transform(Mat4x4 matrix) noexcept 
: Transform() {
    XMFLOAT4X4 matrix_;
    XMStoreFloat4x4(&matrix_, matrix);

    // Extract translation
    translation.x = matrix_._41;
    translation.y = matrix_._42;
    translation.z = matrix_._43;

    // Zero translation part
    matrix_._41 = 0.f;
    matrix_._42 = 0.f;
    matrix_._43 = 0.f;

    // Extract scale
    XMFLOAT3 scale_x, scale_y, scale_z;
    scale_x.x = matrix_._11;
    scale_x.y = matrix_._12;
    scale_x.z = matrix_._13;

    scale_y.x = matrix_._21;
    scale_y.y = matrix_._22;
    scale_y.z = matrix_._23;

    scale_z.x = matrix_._31;
    scale_z.y = matrix_._32;
    scale_z.z = matrix_._33;

    const XMVECTOR scale_x_ = XMLoadFloat3(&scale_x);
    const XMVECTOR scale_y_ = XMLoadFloat3(&scale_y);
    const XMVECTOR scale_z_ = XMLoadFloat3(&scale_z);

    const XMVECTOR scale_x_mag_ = XMVector3Length(scale_x_);
    const XMVECTOR scale_y_mag_ = XMVector3Length(scale_y_);
    const XMVECTOR scale_z_mag_ = XMVector3Length(scale_z_);

    XMStoreFloat(&scaling.x, scale_x_mag_);
    XMStoreFloat(&scaling.y, scale_y_mag_);
    XMStoreFloat(&scaling.z, scale_z_mag_);

    XMFLOAT4X4 rotation_matrix;
    rotation_matrix._11 = matrix_._11 / scaling.x;
    rotation_matrix._21 = matrix_._21 / scaling.y;
    rotation_matrix._31 = matrix_._31 / scaling.z;
    rotation_matrix._41 = 0.f;

    rotation_matrix._12 = matrix_._12 / scaling.x;
    rotation_matrix._22 = matrix_._22 / scaling.y;
    rotation_matrix._32 = matrix_._32 / scaling.z;
    rotation_matrix._42 = 0.f;

    rotation_matrix._13 = matrix_._13 / scaling.x;
    rotation_matrix._23 = matrix_._23 / scaling.y;
    rotation_matrix._33 = matrix_._33 / scaling.z;
    rotation_matrix._43 = 0.f;

    rotation_matrix._14 = 0.f;
    rotation_matrix._24 = 0.f;
    rotation_matrix._34 = 0.f;
    rotation_matrix._44 = 1.f;

    const XMMATRIX rotation_matrix_ = XMLoadFloat4x4(&rotation_matrix);
    rotation = XMQuaternionRotationMatrix(rotation_matrix_);
}

void Transform::reset() noexcept {
    rotation = XMQuaternionIdentity();
    translation = Vec3{0.f, 0.f, 0.f};
    scaling = Vec3{1.f, 1.f, 1.f};
}

Mat4x4 Transform::local_to_world() const noexcept {
    const Mat4x4 translation_matrix = XMMatrixTranslation(translation.x, translation.y, translation.z);
    const Mat4x4 rotation_matrix = XMMatrixRotationQuaternion(rotation);
    const Mat4x4 scaling_matrix = XMMatrixScaling(scaling.x, scaling.y, scaling.z);

    return scaling_matrix  * rotation_matrix * translation_matrix;
}

Mat4x4 Transform::world_to_local() const noexcept {
    return XMMatrixInverse(nullptr, local_to_world());
}

Vec3 Transform::right() const noexcept {
    XMVECTOR direction_cal = XMLoadFloat3(&RightVector);
    direction_cal = XMVector3Rotate(direction_cal, rotation);

    Vec3 new_direction;
    XMStoreFloat3(&new_direction, direction_cal);

    return new_direction;
}

Vec3 Transform::left() const noexcept {
    XMVECTOR direction_cal = XMLoadFloat3(&RightVector) * -1.f;
    direction_cal = XMVector3Rotate(direction_cal, rotation);

    Vec3 new_direction;
    XMStoreFloat3(&new_direction, direction_cal);

    return new_direction;
}

Vec3 Transform::up() const noexcept {
    XMVECTOR direction_cal = XMLoadFloat3(&UpVector);
    direction_cal = XMVector3Rotate(direction_cal, rotation);

    Vec3 new_direction;
    XMStoreFloat3(&new_direction, direction_cal);

    return new_direction;
}

Vec3 Transform::down() const noexcept {
    XMVECTOR direction_cal = XMLoadFloat3(&UpVector) * -1.f;
    direction_cal = XMVector3Rotate(direction_cal, rotation);

    Vec3 new_direction;
    XMStoreFloat3(&new_direction, direction_cal);

    return new_direction;
}

Vec3 Transform::forward() const noexcept {
    XMVECTOR direction_cal = XMLoadFloat3(&ForwardVector);
    direction_cal = XMVector3Rotate(direction_cal, rotation);

    Vec3 new_direction;
    XMStoreFloat3(&new_direction, direction_cal);

    return new_direction;
}

Vec3 Transform::backward() const noexcept {
    XMVECTOR direction_cal = XMLoadFloat3(&ForwardVector) * -1.f;
    direction_cal = XMVector3Rotate(direction_cal, rotation);

    Vec3 new_direction;
    XMStoreFloat3(&new_direction, direction_cal);

    return new_direction;
}

Vec3 Transform::eulerAngles() const noexcept {
    const Mat4x4 rotation_matrix = XMMatrixRotationQuaternion(rotation);

    Vec3 eulers{0.f, 0.f, 0.f};

    eulers.x = std::asin(-rotation_matrix._32);
    if (std::cos(eulers.x) > 0.00001f) {
        eulers.y = std::atan2(rotation_matrix._31, rotation_matrix._33);
        eulers.z = std::atan2(rotation_matrix._12, rotation_matrix._22);
    }
    else {
        eulers.y = 0.f;
        eulers.z = std::atan2(-rotation_matrix._21, rotation_matrix._11);
    }

    return eulers;
}

Transform& Transform::look_at(const Vec3& target) noexcept {
    return look_at(target, UpVector);
}

Transform& Transform::look_at(const Vec3& target, const Vec3& up) noexcept {
    const XMVECTOR target_ = XMLoadFloat3(&target);
    const XMVECTOR position_ = XMLoadFloat3(&translation);
    const XMVECTOR target_direction_ = XMVector3Normalize(target_ - position_);
    const XMVECTOR forward_identity_ = XMLoadFloat3(&ForwardVector);

    rotation = math::look_rotation(target_direction_, XMLoadFloat3(&up));

    return *this;
}

Transform& Transform::rotate(float angle, Vec3 axis) noexcept {
    const XMVECTOR axis_ = XMLoadFloat3(&axis);

    const XMVECTOR rotation_ = XMQuaternionRotationAxis(axis_, angle);

    rotation = XMQuaternionMultiply(rotation, rotation_);

    return *this;
}

Vec3 Transform::scale(Vec3 vector_to_scale) const noexcept {
    const XMVECTOR vector_ = XMLoadFloat3(&vector_to_scale);
    const XMVECTOR scaling_ = XMLoadFloat3(&scaling);

    const XMVECTOR scaled_vector_ = vector_ * scaling_;

    Vec3 scaled_vector;
    XMStoreFloat3(&scaled_vector, scaled_vector_);
    return scaled_vector;
}

Vec3 Transform::transform_point(Vec3 point) const noexcept {
    const XMVECTOR point_ = XMLoadFloat3(&point);
    const XMMATRIX matrix_ = local_to_world();

    const XMVECTOR transformed_point_ = XMVector3Transform(point_, matrix_);

    Vec3 transformed_point;
    XMStoreFloat3(&transformed_point, transformed_point_);

    return transformed_point;
}

Vec3 Transform::inverse_transform_point(Vec3 point) const noexcept {
    const XMVECTOR point_ = XMLoadFloat3(&point);
    const XMMATRIX matrix_ = world_to_local();

    const XMVECTOR transformed_point_ = XMVector3Transform(point_, matrix_);

    Vec3 transformed_point;
    XMStoreFloat3(&transformed_point, transformed_point_);

    return transformed_point;
}

Transform Transform::operator*(const Transform& t) const noexcept {
    return Transform(XMMatrixMultiply(local_to_world(), t.local_to_world()));
}
