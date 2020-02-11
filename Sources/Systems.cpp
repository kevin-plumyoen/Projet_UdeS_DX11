#include "Systems.h"
#include "Components.h"

#include "UpdateRigidBodyTransformsSystem.h"
#include "StaticMeshRenderingSystem.h"
#include "InputSystem.h"
#include "UpdateSystem.h"
#include "CameraSystem.h"
#include "TriggerSystem.h"
#include "BillboardSystem.h"
#include "BillboardRenderingSystem.h"
#include "LightingSystem.h"
#include "HierarchicalTransformSystem.h"
#include "VariableColorUpdateSystem.h"
#include "SplineFollowingSystem.h"
#include "Renderer.h"
#include "Scene.h"

Systems::Systems(Components& components, RequiredValues required, Scene& scene)
: update_rigidbody_transforms{std::make_unique<UpdateRigidBodyTransformsSystem>(components.transforms, required.hierarchy, components.rigidbodies)} 
, render_static_meshes{std::make_unique<StaticMeshRenderingSystem>(required.renderer, components.static_meshes, components.transforms, scene)}
, inputs{std::make_unique<InputSystem>(components.inputs,EntityProxyFactory(components))}
, updates{std::make_unique<UpdateSystem>(components.updates)}
, update_cameras{std::make_unique<CameraSystem>(required.renderer.cameras(), components.cameras, components.transforms)}
, triggers{std::make_unique<TriggerSystem>(required.physic, components.triggers, components.rigidbodies)}
, update_billboards{std::make_unique<BillboardSystem>(components.transforms, components.billboards, required.renderer.cameras())}
, render_billboards{std::make_unique<BillboardRenderingSystem>(required.renderer, components.transforms, components.billboards, scene)}
, lights{std::make_unique<LightingSystem>(components.transforms, components.lights, required.renderer.lights())}
, update_transforms_hiearchicaly(std::make_unique<HierarchicalTransformSystem>(components.transforms, required.hierarchy))
, variable_colors(std::make_unique<VariableColorUpdateSystem>(components.variable_colors,components.lights, components.static_meshes))
, follow_spline(std::make_unique<SplineFollowingSystem>(components.transforms, required.hierarchy, components.splines))
{ }

void Systems::cleanup() {
    update_rigidbody_transforms->cleanup();
    render_static_meshes->cleanup();
    inputs->cleanup();
    updates->cleanup();
    update_cameras->cleanup();
    triggers->cleanup();
    update_billboards->cleanup();
    render_billboards->cleanup();
    lights->cleanup();
}