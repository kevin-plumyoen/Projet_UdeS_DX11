#ifndef DEF_ECS_GRAPPLING_COMPONENT_H
#define DEF_ECS_GRAPPLING_COMPONENT_H
#include "CollisionMasks.h"


struct GrapplingComponent {
	constexpr uint32_t operator()() const noexcept
	{
		return CollisionMasks::GrappableBit;
	}
};

#endif