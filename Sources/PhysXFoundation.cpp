#include "PhysXFoundation.h"

#include <PxPhysics.h>
#include <PxPhysicsVersion.h>
#include <foundation/PxFoundationVersion.h>
#include <foundation/PxFoundation.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <common/PxTolerancesScale.h>
#include <PxPhysicsAPI.h>

#include "Logger.h"

using namespace physx;

#define PVD_HOST "127.0.0.1"

PhysXFoundation::PhysXFoundation(PxAllocatorCallback& allocator, PxErrorCallback& error) 
: foundation{PxCreateFoundation(PX_FOUNDATION_VERSION, allocator, error)}
, pvd{PxCreatePvd(*foundation)} {
    if(!foundation) {
        throw FailedToCreatePhysXFoundation{};
    }
    
    if (!pvd) {
        throw FailedToCreatePhysXPvd{};
    }

    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    if(!transport) {
        throw FailedToCreatePhysXPvd{};
    }
    
    pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    is_connected = transport->isConnected();
    if(is_connected) {
        log_info("engine.physic", "is connected to PVD");
    }
    else {
        log_info("engine.physic", "is not connected to PVD");
    }
}

PhysXFoundation::~PhysXFoundation() {
#if 0
    PxCloseExtensions();
#endif

    pvd->release();
    foundation->release();
}

PhysXPhysics PhysXFoundation::make_physics() const {
    PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
    if (!physics) {
        throw FailedToCreatePhysXPhysics{};
    }

#if 0
    if (!PxInitExtensions(*physics, pvd)) {
        throw FailedToInitializeExtensions{};
    }
#endif

    return PhysXPhysics(physics, is_connected);
}

