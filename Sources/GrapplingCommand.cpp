#include "GrapplingCommand.h"

#include "EntityProxy.h"
#include "EventBus.h"

GrapplingCommand::GrapplingCommand(GrapplingEvent::Grappling grappling, GrapplingEvent::Event event)
: grappling{grappling}
, event{event} {

}

void GrapplingCommand::execute(EntityProxy&, const FrameDuration&) {
    EventBus::instance().transmit(GrapplingEvent(event, grappling));
}