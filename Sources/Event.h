#ifndef DEF_EVENT_H
#define DEF_EVENT_H

#include <functional>
#include <cstdint>
#include <algorithm>

template<typename... Args>
class Event {
public:
    using handler_fn = std::function<void(Args&&...)>;
    using handler_handle = std::size_t;
private:
    std::vector<bool> actives;
    std::vector<handler_fn> handlers;

    std::size_t next_active_handle() const noexcept {
        auto it = std::find(actives.begin(), actives.end(), false);

        return std::distance(actives.begin(), it);
    }

public:
    void broadcast(Args&&... arguments) {
        for (std::size_t i = 0; i < actives.size(); ++i) {
            if (actives[i]) {
                handlers[i](std::forward<Args>(arguments)...);
            }
        }
    }

    handler_handle register_handler(handler_fn fn) {
        const std::size_t next_active = next_active_handle();

        if (next_active == actives.size()) {
            actives.push_back(true);
            handlers.push_back(std::move(fn));
        }
        else {
            actives[next_active] = true;
            handlers[next_active] = std::move(fn);
        }

        return next_active;
    }

    bool is_registered(handler_handle hdl) const noexcept {
        return hdl < actives.size() && actives[hdl];
    }

    void unregister_handler(handler_handle hdl) {
        if (is_registered(hdl)) {
            actives[hdl] = false;
        }
    }
};

#endif