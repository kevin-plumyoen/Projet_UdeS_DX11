#ifndef INPUTCOMMAND_H
#define INPUTCOMMAND_H

#include "Clock.h"

struct EntityProxy;

struct InputCommand {
  virtual void execute(EntityProxy&, const FrameDuration&) = 0;
};

#endif