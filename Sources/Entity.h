#ifndef DEF_ECS_ENTITY_H
#define DEF_ECS_ENTITY_H

#include <cstdint>
#include <limits>
#include <functional>

#undef max

class Entity {
public:
    using generation_type = uint16_t;
    using index_type = uint16_t;
private:
    index_type index_;
    generation_type generation_;
public:
    // Constructors
    constexpr Entity() noexcept
    : index_{ std::numeric_limits<index_type>::max() }
    , generation_{ std::numeric_limits<generation_type>::max() } {

    }

    constexpr Entity(index_type index, generation_type generation) noexcept
    : index_{index}
    , generation_{generation} {

    }

    // Getters
    constexpr index_type index() const noexcept { return index_; }
    constexpr generation_type generation() const noexcept { return generation_; }

    bool valid() const noexcept { return index() != std::numeric_limits<index_type>::max(); }

    // Comparison operators
    constexpr bool operator==(const Entity& other) const noexcept { return index() == other.index() && generation() == other.generation(); }
    constexpr bool operator!=(const Entity& other) const noexcept { return !(*this == other); }
    constexpr bool operator<(const Entity& other) const noexcept { return index() < other.index(); }
    constexpr bool operator<=(const Entity& other) const noexcept { return index() <= other.index(); }
    constexpr bool operator>(const Entity& other) const noexcept { return index() > other.index(); }
    constexpr bool operator>=(const Entity& other) const noexcept { return index() >= other.index(); }
};

struct EntityHash {
    std::size_t operator()(Entity e) const noexcept {
        const uint32_t* raw_bytes = reinterpret_cast<const uint32_t*>(&e);

        return std::hash<uint32_t>()(*raw_bytes);
    }
};

#endif