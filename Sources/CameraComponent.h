#ifndef DEF_CAMERA_COMPONENT_H
#define DEF_CAMERA_COMPONENT_H

#include "CameraManager.h"
#include <memory>

struct CameraLen;

class CameraSystem;

struct CameraComponent {
    friend CameraSystem;
    using LenPtr = std::shared_ptr<CameraLen>;
    LenPtr len = {};
    bool active = false;
private:
    CameraManager::camera_handle handle_ = CameraManager::invalid_handle;
public:
    CameraComponent() = default;
    CameraComponent(LenPtr len)
    : len{ len }
    , active{false} {

    }

    CameraManager::camera_handle handle() const noexcept { return handle_; }
};

#endif