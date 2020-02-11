#include "RequestCameraChangeCommand.h"

#include "EventBus.h"

void RequestCameraChangeCommand::execute(EntityProxy&, const FrameDuration&) {
    EventBus::instance().transmit(CameraChangeEvent{});
}