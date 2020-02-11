#ifndef KEYBOARDMOVEMENTCONTROLLER_H
#define KEYBOARDMOVEMENTCONTROLLER_H

#include "InputController.h"

class KeyboardMovementController : public InputController {
  InputCommand* Up;
  InputCommand* Down;
  InputCommand* Right;
  InputCommand* Left;
  InputCommand* Jump;

public:
    KeyboardMovementController(PM3D::CDIManipulateur* direct_input, InputCommand* up = nullptr, InputCommand* down = nullptr, InputCommand* right = nullptr, InputCommand* left = nullptr, InputCommand* jump = nullptr);
    void getCommands() override;

    void set_inputs(InputCommand* up, InputCommand* down, InputCommand* right, InputCommand* left, InputCommand* jump);
};

#endif
