#include "JumpCommand.h"

#include "EntityProxy.h"
#include "RigidBodyComponent.h"
#include "Logger.h"
#include "PhysicEngine.h"
#include <PxRigidDynamic.h>

using namespace physx;

JumpCommand::JumpCommand(const PhysicEngine& physic) 
: physic{physic} {

}

void JumpCommand::execute(EntityProxy& entity, const FrameDuration&) {
    PxRigidDynamic* rigid_dynamic =  entity.rigid_body->rigidbody->is<PxRigidDynamic>();

    if (rigid_dynamic) {
        // Only jump if grounded
        if(physic.is_grounded(rigid_dynamic)) {
            rigid_dynamic->addForce(PxVec3{0.f, 5000.f, 0.f}, PxForceMode::eIMPULSE);
        }
    }
    else {
        log_warning("game.jump_command", "Associated rigid actor cannot be casted to PxRigidDynamic");
    }
}