#include "CameraManager.h"

#include <limits>
#include <cassert>

const CameraManager::camera_handle CameraManager::invalid_handle = std::numeric_limits<camera_handle>::max();

CameraManager::camera_handle CameraManager::next_handle() {
    const auto it = std::find(handles.begin(), handles.end(), invalid_handle);

    if(it != std::end(handles)) {
        return std::distance(handles.begin(), it);
    }

    handles.push_back(invalid_handle);
    return handles.size() - 1;
}

CameraManager::camera_handle CameraManager::handle_to_last_camera() const noexcept {
    const std::size_t last_index = cameras.size() - 1;

    const auto it = std::find(handles.begin(), handles.end(), last_index);

    if(it != std::end(handles)) {
        return std::distance(handles.begin(), it);
    }

    return invalid_handle;
}

CameraManager::camera_handle CameraManager::make(std::shared_ptr<CameraLen> len, const Camera::vector_type& position, const Camera::vector_type& direction) {
    const camera_handle hdl = next_handle();
    handles[hdl] = cameras.size();
    cameras.emplace_back(len, position, direction);

    return hdl;
}

CameraManager::camera_handle CameraManager::make(std::shared_ptr<CameraLen> len, const Camera::vector_type& position, const Camera::vector_type& direction, const Camera::vector_type& up) {
    const camera_handle hdl = next_handle();
    handles[hdl] = cameras.size();
    cameras.emplace_back(len, position, direction, up);

    return hdl;
}

void CameraManager::destroy(camera_handle handle) {
    if (handle < handles.size() && handles[handle] != invalid_handle) {
        const std::size_t camera_index_to_remove = handles[handle];
        handles[handle] = invalid_handle;

        const camera_handle handle_to_last = handle_to_last_camera();
        if (handle_to_last != invalid_handle) {
            handles[handle_to_last] = camera_index_to_remove;
        }

        std::swap(cameras[camera_index_to_remove], cameras.back());
        cameras.pop_back();
    }
}

bool CameraManager::exists(camera_handle handle) const noexcept {
    return handle < handles.size() && handles[handle] != invalid_handle;
}

void CameraManager::overload_rendering_camera(Camera* camera) {
    rendering_camera_overload = camera;
}

void CameraManager::set_active(camera_handle handle) {
    if (exists(handle)) {
        active_camera_ = handle;
    }
    else {
        throw InvalidHandle{};
    }
}

void CameraManager::reset_active() {
    active_camera_ = invalid_handle;
}

void CameraManager::reset() {
    reset_active();
    cameras.clear();
    handles.clear();
}

bool CameraManager::has_active() const noexcept {
    return exists(active_camera_);
}

Camera& CameraManager::active_camera() noexcept {
    return (*this)[active_camera_];
}

const Camera& CameraManager::active_camera() const noexcept {
    return (*this)[active_camera_];
}

Camera& CameraManager::rendering_camera() noexcept {
    if(rendering_camera_overload) return *rendering_camera_overload;

    return active_camera();
}

const Camera& CameraManager::rendering_camera() const noexcept {
    if(rendering_camera_overload) return *rendering_camera_overload;

    return active_camera();
}

Camera& CameraManager::operator[](camera_handle handle) noexcept {
    assert(exists(handle));

    return cameras[handles[handle]];
}

const Camera& CameraManager::operator[](camera_handle handle) const noexcept {
    assert(exists(handle));

    return cameras[handles[handle]];
}

CameraManager::iterator CameraManager::begin() noexcept {
    return cameras.begin();
}

CameraManager::iterator CameraManager::end() noexcept {
    return cameras.end();
}

CameraManager::const_iterator CameraManager::begin() const noexcept {
    return cameras.begin();
}

CameraManager::const_iterator CameraManager::end() const noexcept {
    return cameras.end();
}

CameraManager::const_iterator CameraManager::cbegin() const noexcept {
    return begin();
}

CameraManager::const_iterator CameraManager::cend() const noexcept {
    return end();
}