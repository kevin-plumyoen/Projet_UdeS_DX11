#include "Camera.h"
#include "CameraLen.h"

static std::shared_ptr<CameraLen> required_camera_len(std::shared_ptr<CameraLen> len) {
    return len != nullptr ? len : throw MissingCameraLen{};
}

Camera::Camera(std::shared_ptr<CameraLen> len, const vector_type& position, const vector_type& direction)
: Camera(len, position, direction, {0.f, 0.f, 1.f}) {

}

Camera::Camera(std::shared_ptr<CameraLen> len, const vector_type& position, const vector_type& direction, const vector_type& up) 
: len{ required_camera_len(len) }
, position{ position }
, direction{ direction }
, up{ up } {

}

Camera::~Camera() = default;

void Camera::setPosition(const vector_type& position_in) { 
    position = position_in; 
}

void Camera::setDirection(const vector_type& direction_in) { 
   DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction_in)));
}

void Camera::setUp(const vector_type& up_in) { 
    up = up_in; 
}

Camera::matrix_type Camera::view() const noexcept {
    const DirectX::XMVECTOR pos = DirectX::XMVectorSet(position.x, position.y, position.z, 1.f);
    const DirectX::XMVECTOR dir = DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.f);
    const DirectX::XMVECTOR up_ = DirectX::XMVectorSet(up.x, up.y, up.z, 0.f);

    return DirectX::XMMatrixLookAtLH(pos, DirectX::XMVectorAdd(pos, dir), up_);
}

Camera::matrix_type Camera::projection() const noexcept {
    return len->projection();
}

Camera::vector_type Camera::getPosition() const noexcept {
    return position; 
}

Camera::vector_type Camera::getDirection() const noexcept { 
    return direction; 
}

Camera::vector_type Camera::getUp() const noexcept { 
    return up; 
}

Frustum Camera::frustum() const noexcept {
    return len->frustum(*this);
}

CameraRay Camera::ray_from_viewport_coord(const Vec2& coord) const noexcept {
    using namespace DirectX;
    assert(coord.x <= 1.f);
    assert(coord.y <= 1.f);
    assert(coord.x >= -1.f);
    assert(coord.y >= -1.f);
    
    const XMVECTOR ray_clip_space_ = XMVectorSet(coord.x, coord.y, 1.f, 1.f);
    const XMMATRIX inverse_projection_ = XMMatrixInverse(nullptr, projection());
    const XMVECTOR ray_eye_space_ = XMVector4Transform(ray_clip_space_, inverse_projection_);
    XMFLOAT4 ray_eye_space;
    XMStoreFloat4(&ray_eye_space, ray_eye_space_);
    ray_eye_space.z = 1.f;
    ray_eye_space.w = 0.f;

    const XMVECTOR ray_eye_space_direction_ = XMLoadFloat4(&ray_eye_space);
    const XMMATRIX inverse_view_ = XMMatrixInverse(nullptr, view());
    const XMVECTOR ray_world_space_ = XMVector4Transform(ray_eye_space_direction_, inverse_view_);
    const XMVECTOR ray_world_space_direction_ = XMVector3Normalize(ray_world_space_);

    XMFLOAT3 ray_world_space_direction;
    XMStoreFloat3(&ray_world_space_direction, ray_world_space_direction_);

    return CameraRay(position, ray_world_space_direction);
}