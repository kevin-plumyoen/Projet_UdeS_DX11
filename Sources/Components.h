#ifndef DEF_ECS_COMPONENTS_H
#define DEF_ECS_COMPONENTS_H

#include "ComponentStorages.h"

struct Components {
    TransformStorage transforms;
    RigidBodyStorage rigidbodies;
    StaticMeshStorage static_meshes;
    CameraStorage cameras;
    LightStorage lights;
    InputStorage inputs;
    UpdateStorage updates;
    TriggerStorage triggers;
    BillboardStorage billboards;
	GrapplingStorage grapplings;
	VariableColorStorage variable_colors;
    SplineStorage splines;

    void cleanup();
};

#endif