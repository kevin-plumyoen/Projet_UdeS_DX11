#ifndef DEF_CAMERA_EVENT_H
#define DEF_CAMERA_EVENT_H

#include "EventTraits.h"
#include <cstdint>

// Request a camera change
struct CameraChangeEvent {};

template<>
struct is_event<CameraChangeEvent> : std::true_type {};

#endif