#include "DebugCameraController.h"
#include "Camera.h"
#include "DIManipulateur.h"

DebugCameraController::DebugCameraController(Camera& camera)
: camera{camera} {

}

void DebugCameraController::handle_input(const PM3D::CDIManipulateur& input, FrameDuration dt) {
    const std::chrono::duration<float> temps_ecoule_secs = std::chrono::duration_cast<std::chrono::duration<float>>(dt);
    
    const DirectX::XMFLOAT3 cam_up = camera.getUp();
    const DirectX::XMVECTOR cam_up_ = DirectX::XMLoadFloat3(&cam_up);

    const DirectX::XMFLOAT3 cam_direction = camera.getDirection();
    DirectX::XMVECTOR cam_direction_ = DirectX::XMLoadFloat3(&cam_direction);
    
    const float rotate_factor = 2.6f;

    // Gère direction
    auto q1 = DirectX::XMQuaternionRotationAxis(cam_up_, temps_ecoule_secs.count() * -rotate_factor);
    auto q2 = DirectX::XMQuaternionRotationAxis(cam_up_, temps_ecoule_secs.count() * rotate_factor);
    auto q3 = DirectX::XMQuaternionRotationAxis(XMVector3Cross(cam_direction_, cam_up_), temps_ecoule_secs.count() * rotate_factor);
    auto q4 = DirectX::XMQuaternionRotationAxis(XMVector3Cross(cam_direction_, cam_up_), temps_ecoule_secs.count() * -rotate_factor);

    if (input.EtatSouris().x() < 0)
    {
        cam_direction_ = XMVector3Rotate(cam_direction_, q1);
    }
    // Vérifier si déplacement vers la droite
    else if (input.EtatSouris().x() > 0)
    {
        cam_direction_ = XMVector3Rotate(cam_direction_, q2);
    }
    // Vérifier si déplacement vers le haut
    if ((input.EtatSouris().y() < 0) && DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(cam_direction_, cam_up_)) > 0.1f)
    {
        cam_direction_ = XMVector3Rotate(cam_direction_, q3);
    }
    // Vérifier si déplacement vers le bas
    else if ((input.EtatSouris().y() > 0) && DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(cam_direction_, cam_up_)) < DirectX::XM_PI - 0.1f)
    {
        cam_direction_ = XMVector3Rotate(cam_direction_, q4);
    }

    DirectX::XMFLOAT3 direction;
    XMStoreFloat3(&direction, cam_direction_);

    camera.setDirection(direction);

    // Gère position
    const DirectX::XMFLOAT3 cam_position = camera.getPosition();

    const DirectX::XMVECTOR cam_position_ = DirectX::XMLoadFloat3(&cam_position);
    
    const DirectX::XMVECTOR cam_right_ = DirectX::XMVector3Cross(cam_up_, cam_direction_);

    DirectX::XMVECTOR movement_direction = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);

    if (input.ToucheAppuyee(KeyboardKey::a)) {
        movement_direction -= cam_right_; //cam_position_ - cam_right_;
    }
            
    if (input.ToucheAppuyee(KeyboardKey::d)) {
        movement_direction += cam_right_;
    }

    if (input.ToucheAppuyee(KeyboardKey::s)) {
        movement_direction -= cam_direction_;
    }

    if (input.ToucheAppuyee(KeyboardKey::w)) {
        movement_direction += cam_direction_;
    }

    movement_direction = DirectX::XMVector3Normalize(movement_direction)*2;

    DirectX::XMFLOAT3 new_cam_position;
    DirectX::XMStoreFloat3(&new_cam_position, cam_position_ + movement_direction);

    camera.setPosition(new_cam_position);
}

void DebugCameraController::reset_to(const Camera& cam) {
    camera.setPosition(cam.getPosition());
    camera.setDirection(cam.getDirection());
    camera.setUp(cam.getUp());
}