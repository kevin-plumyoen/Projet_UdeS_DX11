#ifndef DEF_PHYSIC_HELPERS_H
#define DEF_PHYSIC_HELPERS_H

#include <cstdint>

namespace physx {
    class PxRigidActor;
}

namespace PhysicHelper {

void set_query_filter_mask(physx::PxRigidActor* actor, uint32_t mask);

void attach_user_data(physx::PxRigidActor* actor, void* userdata);
void* attached_user_data(physx::PxRigidActor* actor);

}

#endif