#ifndef DEF_ECS_RIGIDBODY_COMPONENT_H
#define DEF_ECS_RIGIDBODY_COMPONENT_H

namespace physx {
   
class PxRigidActor;

}

struct RigidBodyComponent {
    physx::PxRigidActor* rigidbody;
    bool updateTransform = false;

    constexpr RigidBodyComponent() noexcept
       : rigidbody{ nullptr }, updateTransform{ false } {

    }

    constexpr RigidBodyComponent(physx::PxRigidActor* rigidbody) noexcept
    : rigidbody{ rigidbody }, updateTransform{ false } {

    }
};

#endif