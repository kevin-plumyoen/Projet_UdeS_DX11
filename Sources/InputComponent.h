#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include <memory>

class InputController;

struct InputComponent {
public:
    std::shared_ptr<InputController> controller;

    InputComponent(std::shared_ptr<InputController> ctrl) noexcept
    : controller{ ctrl } {

    }
};

#endif