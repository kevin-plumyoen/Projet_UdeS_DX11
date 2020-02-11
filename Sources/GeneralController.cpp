#include "GeneralController.h"

#include "DIManipulateur.h"
#include "InputCommand.h"

GeneralController::GeneralController(PM3D::CDIManipulateur* direct_input, InputCommand* quit) 
: InputController(direct_input)
, Quit{quit} {

}

void GeneralController::getCommands() {
    if (inputs->ToucheAppuyee(ControllerLayout::get().pause)) {
      commands.push_back(Quit);
    }
}