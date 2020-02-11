#ifndef DEF_GRAPPLING_COMMAND_H
#define DEF_GRAPPLING_COMMAND_H

#include "InputCommand.h"
#include "GrapplingEvent.h"

class GrapplingCommand : public InputCommand {
    GrapplingEvent::Grappling grappling;
    GrapplingEvent::Event event;
public:
    GrapplingCommand(GrapplingEvent::Grappling grappling, GrapplingEvent::Event event);

    void execute(EntityProxy&, const FrameDuration&) override;
};

#endif