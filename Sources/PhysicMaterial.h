#ifndef DEF_PHYSIC_MATERIAL_H
#define DEF_PHYSIC_MATERIAL_H

#include <optional>

class Percent {
    float value_;

    constexpr static float is_in_range(const float value) {
        return (value >= 0.f && value <= 1.f) ? value : throw OutOfRange{};
    }

public:
    struct OutOfRange {};

    constexpr Percent(float value)
    : value_{ is_in_range(value) } {

    }

    operator float() const noexcept { return value_; }
};

struct PhysicMaterial {
    Percent static_friction;
    Percent dynamic_friction;
    Percent restitution;
    float mass;

    constexpr PhysicMaterial() noexcept
    : static_friction{ 0.f }
    , dynamic_friction{ 0.f }
    , restitution{ 0.f }
    , mass{0.f} {

    }

    constexpr PhysicMaterial(float static_friction, float dynamic_friction, float restitution) noexcept
    : static_friction{ static_friction }
    , dynamic_friction{ dynamic_friction }
    , restitution{ restitution }
    , mass{0.f} {

    }

    constexpr PhysicMaterial(float static_friction, float dynamic_friction, float restitution, float mass) noexcept
    : static_friction{ static_friction }
    , dynamic_friction{ dynamic_friction }
    , restitution{ restitution }
    , mass{mass} {

    }
};

#endif