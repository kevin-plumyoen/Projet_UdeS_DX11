#ifndef DEF_PHYSX_PHYSICS_H
#define DEF_PHYSX_PHYSICS_H

namespace physx {

class PxPhysics;
class PxDefaultCpuDispatcher;
class PxScene;

}

class PhysXFoundation;

struct FailedToCreatePhysXScene{};
struct FailedToCreateCpuDispatcher{};

class PhysXPhysics
{
    friend PhysXFoundation;
    physx::PxPhysics* physics;
    physx::PxDefaultCpuDispatcher* cpu_dispatcher;
    bool connected_to_pvd;

    explicit PhysXPhysics(physx::PxPhysics* physics, bool connected);
public:
    constexpr PhysXPhysics() noexcept
    : physics{ nullptr }
    , cpu_dispatcher{ nullptr }
    , connected_to_pvd{false} {

    }

    // Copy is disabled
    PhysXPhysics(const PhysXPhysics&) = delete;
    PhysXPhysics& operator=(const PhysXPhysics&) = delete;
    PhysXPhysics(PhysXPhysics&& physics) noexcept;
    PhysXPhysics& operator=(PhysXPhysics&& physics) noexcept;

    ~PhysXPhysics() noexcept;

    void swap(PhysXPhysics& other) noexcept;

    physx::PxScene* make_scene();

    constexpr operator bool() const noexcept {
        return physics != nullptr && cpu_dispatcher != nullptr;
    }

    bool is_connected() const noexcept { return connected_to_pvd; }

    physx::PxPhysics& getPhysics();
};

#endif