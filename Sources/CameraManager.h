#ifndef DEF_CAMERA_MANAGER_H
#define DEF_CAMERA_MANAGER_H

#include "Camera.h"
#include <cstdint>
#include <vector>
#include <memory>

class CameraManager {
public:
    struct InvalidHandle{};

    using camera_handle = std::size_t;
    static const camera_handle invalid_handle;
private:
    std::vector<std::size_t> handles = {};
    std::vector<Camera> cameras = {};
    Camera* rendering_camera_overload = nullptr;
    camera_handle active_camera_ = invalid_handle;
    camera_handle next_handle();
    camera_handle handle_to_last_camera() const noexcept;
public:
    using iterator = std::vector<Camera>::iterator;
    using const_iterator = std::vector<Camera>::const_iterator;

    CameraManager() = default;

    camera_handle make(std::shared_ptr<CameraLen> len, const Camera::vector_type& position = Camera::vector_type{0.f, 0.f, 0.f}, const Camera::vector_type& direction = Camera::vector_type{0.f, 1.f, 0.f});
    camera_handle make(std::shared_ptr<CameraLen> len, const Camera::vector_type& position, const Camera::vector_type& direction, const Camera::vector_type& up);
    void destroy(camera_handle handle);

    bool exists(camera_handle handle) const noexcept;

    void overload_rendering_camera(Camera* camera);

    void set_active(camera_handle handle);
    void reset_active();
    void reset();
    bool has_active() const noexcept;
    Camera& active_camera() noexcept;
    const Camera& active_camera() const noexcept;

    Camera& rendering_camera() noexcept;
    const Camera& rendering_camera() const noexcept;

    Camera& operator[](camera_handle handle) noexcept;
    const Camera& operator[](camera_handle handle) const noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;

    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
};

#endif