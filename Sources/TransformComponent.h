#ifndef DEF_ECS_TRANSFORM_COMPONENT_H
#define DEF_ECS_TRANSFORM_COMPONENT_H

#include "Transform.h"

struct TransformComponent {
    Transform local;
    Transform world;
};

#endif