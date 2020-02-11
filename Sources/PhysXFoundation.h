#ifndef DEF_PHYSX_FOUNDATION_H
#define DEF_PHYSX_FOUNDATION_H

#include "PhysXPhysics.h"

namespace physx {
class PxFoundation;
class PxAllocatorCallback;
class PxErrorCallback;
class PxPvd;
}

class FailedToCreatePhysXFoundation{};
class FailedToCreatePhysXPhysics{};
class FailedToCreatePhysXPvd{};
class FailedToInitializeExtensions{};

class PhysXFoundation {
    physx::PxFoundation* foundation;
    physx::PxPvd* pvd;
    bool is_connected;
public:
    PhysXFoundation(physx::PxAllocatorCallback& allocator, physx::PxErrorCallback& error);
    ~PhysXFoundation();

    // Copy is disabled
    PhysXFoundation(const PhysXFoundation&) = delete;
    PhysXFoundation& operator=(const PhysXFoundation&) = delete;

    PhysXPhysics make_physics() const;

    physx::PxPvd& getPvd() { return *pvd; }
    physx::PxFoundation& getFoundation() { return *foundation; };
};

#endif