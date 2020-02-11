#ifndef ENTITYPROXY_H
#define ENTITYPROXY_H

struct InputComponent;
struct LightComponent;
struct CameraComponent;
struct TransformComponent;
struct RigidBodyComponent;
struct StaticMeshComponent;

struct EntityProxy {
  EntityProxy() = default;
  EntityProxy(EntityProxy&& e) = default;
  EntityProxy(EntityProxy&) = default;
  EntityProxy& operator=(EntityProxy&) = default;
  EntityProxy&& operator=(EntityProxy&& e) { return std::move(e); }

  InputComponent* input;
  LightComponent* light;
  CameraComponent* camera;
  TransformComponent* transform;
  RigidBodyComponent* rigid_body;
  StaticMeshComponent* static_mesh;
};

#endif