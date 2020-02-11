#ifndef DEF_JUMP_COMMAND_H
#define DEF_JUMP_COMMAND_H

#include "InputCommand.h"

class PhysicEngine;

class JumpCommand : public InputCommand {
    const PhysicEngine& physic;
public:
    JumpCommand(const PhysicEngine& physic);
    void execute(EntityProxy&, const FrameDuration&) override;
};

#endif