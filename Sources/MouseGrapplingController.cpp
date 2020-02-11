#include "MouseGrapplingController.h"

#include "DIManipulateur.h"

#include "GrapplingEvent.h"
#include "EventBus.h"
#include "GrapplingCommand.h"

MouseGrapplingController::MouseGrapplingController(PM3D::CDIManipulateur* direct_input) 
: InputController(direct_input)
, attach_left_grappling_cmd{std::make_unique<GrapplingCommand>( GrapplingEvent::Grappling::Left,  GrapplingEvent::Event::Attached)}
, attach_right_grappling_cmd{std::make_unique<GrapplingCommand>(GrapplingEvent::Grappling::Right, GrapplingEvent::Event::Attached)}
, detach_left_grappling_cmd{std::make_unique<GrapplingCommand>( GrapplingEvent::Grappling::Left,  GrapplingEvent::Event::Detached)}
, detach_right_grappling_cmd{std::make_unique<GrapplingCommand>(GrapplingEvent::Grappling::Right, GrapplingEvent::Event::Detached)}
, is_left_grappling_attached{false}
, is_right_grappling_attached{false} {

}

void MouseGrapplingController::attach_left_grappling() {
    commands.push_back(attach_left_grappling_cmd.get());
    is_left_grappling_attached = true;
}

void MouseGrapplingController::detach_left_grappling() {
    commands.push_back(detach_left_grappling_cmd.get());
    is_left_grappling_attached = false;
}

void MouseGrapplingController::attach_right_grappling() {
    commands.push_back(attach_right_grappling_cmd.get());
    is_right_grappling_attached = true;
}

void MouseGrapplingController::detach_right_grappling() {
    commands.push_back(detach_right_grappling_cmd.get());
    is_right_grappling_attached = false;
}

void MouseGrapplingController::getCommands() {
    // Gestion du grappin gauche
    if (inputs->EtatSouris().left_button_pressed() && !is_left_grappling_attached) {
        attach_left_grappling();
    }
    else if (!inputs->EtatSouris().left_button_pressed() && is_left_grappling_attached) {
        detach_left_grappling();
    }

    // Gestion du grappin droit
    if (inputs->EtatSouris().right_button_pressed() && !is_right_grappling_attached) {
        attach_right_grappling();
    }
    else if (!inputs->EtatSouris().right_button_pressed() && is_right_grappling_attached) {
        detach_right_grappling();
    }
}