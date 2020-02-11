#ifndef DEF_TRIGGER_COMPONENT_H
#define DEF_TRIGGER_COMPONENT_H

#include <cstdint>

struct TriggerComponent {
    using free_callback_fn = void(*)(void*);

    uint32_t id;           // Un entier pour identifier le composant
    void* userdata;        // Donn�es utilisateurs � attach�
    free_callback_fn free; // Fonction � appeler pour relacher le userdata (si n�cessaire)

    constexpr TriggerComponent(uint32_t id) noexcept
    : id{ id }
    , userdata{ nullptr }
    , free{ nullptr } {

    }

    constexpr TriggerComponent(uint32_t id, void* userdata, free_callback_fn free = nullptr) noexcept
    : id{ id }
    , userdata{ userdata }
    , free{ free } {

    }

    TriggerComponent(const TriggerComponent&) = delete;

    constexpr TriggerComponent(TriggerComponent&& other) noexcept
    : id{ other.id }
    , userdata{ other.userdata }
    , free{ other.free } {
        other.userdata = nullptr;
        other.free = nullptr;
    }

    TriggerComponent& operator=(TriggerComponent&& other) noexcept {
        swap(other);

        return *this;
    }

    ~TriggerComponent() {
        if (free) {
            free(userdata);
        }
    }

    void swap(TriggerComponent& other) noexcept {
        using std::swap;

        swap(id, other.id);
        swap(free, other.free);
        swap(userdata, other.userdata);
    }
};

#endif