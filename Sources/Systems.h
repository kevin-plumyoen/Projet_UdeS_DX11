#ifndef DEF_ECS_SYSTEMS_H
#define DEF_ECS_SYSTEMS_H

#include "SystemInterface.h"
#include "Scene.h"

#include <memory>

struct Components;
struct Renderer;
class CameraManager;
class LightManager;
class PhysicEngine;
class TransformHierarchy;

struct Systems {
    using SystemPtr = std::unique_ptr<BaseSystem>;
    struct RequiredValues {
        Renderer& renderer;
        PhysicEngine& physic;
        TransformHierarchy& hierarchy;

        constexpr RequiredValues(Renderer& renderer, PhysicEngine& physic, TransformHierarchy& hierarchy) noexcept
        : renderer{ renderer }
        , physic{ physic }
        , hierarchy{ hierarchy} {

        }
    };

    SystemPtr update_rigidbody_transforms;
    SystemPtr render_static_meshes;
    SystemPtr inputs;
    SystemPtr updates;
    SystemPtr update_cameras;
    SystemPtr triggers;
    SystemPtr update_billboards;
    SystemPtr render_billboards;
    SystemPtr lights;
    SystemPtr update_transforms_hiearchicaly;
    SystemPtr variable_colors;
    SystemPtr follow_spline;

    Systems(Components& components, RequiredValues required, Scene& scene);
    void cleanup();
};

#endif