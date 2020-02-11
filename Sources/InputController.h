#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include "ControllerLayout.h"

#include <vector>

namespace PM3D {
    class CDIManipulateur;
}

struct InputCommand;

class InputController {
public:
    InputController(PM3D::CDIManipulateur* inputs)
    : inputs{ inputs } {
    }

    virtual void getCommands() = 0;
    void clearCommands() { commands.clear(); }

    PM3D::CDIManipulateur* inputs;
    std::vector<InputCommand*> commands;
};

#endif