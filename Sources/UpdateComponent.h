#ifndef DEF_ECS_UPDATE_COMPONENT_H
#define DEF_ECS_UPDATE_COMPONENT_H

#include "Clock.h"
#include <memory>

namespace UpdateFlags {
    enum : uint8_t {
        UpdateBit =             1,       // L'update sera exécuté lors de la phase "update"
        LateUpdateBit =        (1 << 1), // L'update sera exécuté lors de la phase "late update"
        ExecuteWhenPausedBit = (1 << 2), // L'update sera exécuté même si le jeu est en pause
    };
}

struct UpdateComponent {
    using callback_fn = void(*)(FrameDuration, uint8_t, void*);
    using free_callback_fn = void(*)(void*);

    callback_fn callback;
    free_callback_fn free;
    void* userdata;
    uint8_t flags;

    constexpr UpdateComponent(free_callback_fn free = nullptr) noexcept
    : callback{ nullptr }
    , free{free}
    , userdata{ nullptr }
    , flags{ 0 } {

    }

    constexpr UpdateComponent(callback_fn callback, uint8_t flags, free_callback_fn free = nullptr)
    : callback{ callback }
    , free{ free }
    , userdata{ nullptr }
    , flags{ flags } {

    }

    constexpr UpdateComponent(callback_fn callback, uint8_t flags, void* userdata, free_callback_fn free = nullptr)
    : callback{ callback }
    , free{ free }
    , userdata{ userdata }
    , flags{ flags } {

    }

    UpdateComponent(const UpdateComponent&) = delete;
    UpdateComponent& operator=(const UpdateComponent&) = delete;

    UpdateComponent(UpdateComponent&& other) noexcept
    : callback{ other.callback }
    , free{ other.free }
    , userdata{ other.userdata }
    , flags{ other.flags } {
        other.callback = nullptr;
        other.free = nullptr;
        other.userdata = nullptr;
    }

    UpdateComponent& operator=(UpdateComponent&& other) noexcept {
        this->swap(other);
        return *this;
    }

    ~UpdateComponent() noexcept {
        if (free) {
            free(userdata);
        }
    }

    void swap(UpdateComponent& other) noexcept {
        using std::swap;

        swap(callback, other.callback);
        swap(free, other.free);
        swap(userdata, other.userdata);
        swap(flags, other.flags);
    }
};

#endif