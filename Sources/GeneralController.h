#ifndef GENERALCONTROLLER_H
#define GENERALCONTROLLER_H

#include "InputController.h"

class GeneralController : public InputController {
  InputCommand* Quit;

public:
    GeneralController(PM3D::CDIManipulateur* direct_input, InputCommand* quit);
    void getCommands();
};

#endif
