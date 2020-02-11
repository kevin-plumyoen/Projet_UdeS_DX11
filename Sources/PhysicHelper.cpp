#include "PhysicHelpers.h"

#include <PxPhysicsAPI.h>

#include <limits>
#include <vector>
#include <cassert>

using namespace physx;

namespace PhysicHelper {

void set_query_filter_mask(physx::PxRigidActor* actor, uint32_t mask) {
    if (actor) {

        std::vector<PxShape*> shapes(actor->getNbShapes(), nullptr);
        assert(shapes.size() <= std::numeric_limits<PxU32>::max());
        actor->getShapes(shapes.data(), static_cast<PxU32>(shapes.size()), 0);

        PxFilterData filter_data;
        filter_data.word0 = mask;

        for (PxShape* shape : shapes) {
            shape->acquireReference();
            actor->detachShape(*shape);
            shape->setQueryFilterData(filter_data);
            actor->attachShape(*shape);
            shape->release();
        }
    }
}

void attach_user_data(physx::PxRigidActor* actor, void* userdata) {
    if (actor) {
        actor->userData = userdata;
    }
}

void* attached_user_data(physx::PxRigidActor* actor) {
    if (actor) {
        return actor->userData;
    }

    return nullptr;
}

}