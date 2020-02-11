#include "KeyboardMovementController.h"

#include "InputCommand.h"
#include "DIManipulateur.h"

KeyboardMovementController::KeyboardMovementController(PM3D::CDIManipulateur* direct_input, InputCommand* up, InputCommand* down, InputCommand* right, InputCommand* left, InputCommand* jump) 
: InputController{direct_input}
, Up { up }
, Down{ down }
, Right{ right }
, Left{ left }
, Jump{ jump }{
}

void KeyboardMovementController::getCommands() {
    assert(Up);
    assert(Down);
    assert(Left);
    assert(Right);
    assert(Jump);
    if (inputs->ToucheAppuyee(ControllerLayout::get().up)/* || inputs.ToucheAppuyee(layout.alt_up)*/) {
        commands.push_back(Up);
    }
    if (inputs->ToucheAppuyee(ControllerLayout::get().down)/* || inputs.ToucheAppuyee(layout.alt_down)*/) {
        commands.push_back(Down);
    }
    if (inputs->ToucheAppuyee(ControllerLayout::get().right)/* || inputs.ToucheAppuyee(layout.alt_right)*/) {
        commands.push_back(Right);
    }
    if (inputs->ToucheAppuyee(ControllerLayout::get().left)/* || inputs.ToucheAppuyee(layout.alt_left)*/) {
        commands.push_back(Left);
    }
    if (inputs->ToucheAppuyee(ControllerLayout::get().jump)) {
        commands.push_back(Jump);
    }
}

void KeyboardMovementController::set_inputs(InputCommand* up, InputCommand* down, InputCommand* right, InputCommand* left, InputCommand* jump) {
    Up = up;
    Down = down;
    Left = left;
    Right = right;
    Jump = jump;
}