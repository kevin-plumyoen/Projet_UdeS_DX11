#ifndef DEF_DEBUG_PROFILER_H
#define DEF_DEBUG_PROFILER_H

#include <string>
#include <string_view>
#include <chrono>
#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <atomic>

class Profiler {
public:
    using clock = std::chrono::high_resolution_clock;
    
    struct Event {
        std::string name;
        std::string category;
        clock::time_point ts;
        std::thread::id tid;

        Event(std::string_view name) noexcept;
        Event(std::string_view name, std::string_view category) noexcept;
        Event(std::string_view name, std::string_view category, clock::time_point ts) noexcept;

        virtual std::ostream& serialize(std::ostream& out) const;
        friend std::ostream& operator<<(std::ostream& out, const Event& e) {
            return e.serialize(out);
        }
    };

    struct DurationEvent : public Event {
        enum class Type : uint8_t {
            Start,
            End
        };
        Type type;

        DurationEvent(std::string_view name, Type type) noexcept;
        DurationEvent(std::string_view name, std::string_view category, Type type) noexcept;
        virtual std::ostream& serialize(std::ostream& out) const override;
    };

    struct CompleteEvent : public Event {
        std::chrono::microseconds duration;

        CompleteEvent(std::string_view name, clock::time_point start, std::chrono::microseconds duration) noexcept;
        CompleteEvent(std::string_view name, std::string_view category, clock::time_point start, std::chrono::microseconds duration) noexcept;
        virtual std::ostream& serialize(std::ostream& out) const override;
    };

    struct InstantEvent : public Event {
        enum class Scope {
            Global,
            Process,
            Thread
        };
        Scope scope;

        InstantEvent(std::string_view name) noexcept;
        InstantEvent(std::string_view name, std::string_view category) noexcept;
        InstantEvent(std::string_view name, Scope scope) noexcept;
        InstantEvent(std::string_view name, std::string_view category, Scope scope) noexcept;

        virtual std::ostream& serialize(std::ostream& out) const override;
    };

private:
    static std::ostream& write_header(std::ostream& stream);
    static std::ostream& write_footer(std::ostream& stream);

#ifdef ENABLE_PROFILING
    void serialize_events_thread();

    std::atomic<bool> is_running;
    std::thread background_thread;

    std::vector<std::unique_ptr<Event>> events;
    std::mutex m;
#endif
public:
    Profiler();
    ~Profiler();

    // Copy is disabled
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;

    static Profiler& instance();

    void push_event(std::unique_ptr<Event> ev);
    
    template<typename It>
    void push_events(It begin, It end) {
#ifdef ENABLE_PROFILING
        std::lock_guard<std::mutex> lock(m);

        events.insert(events.back(), begin, end);
#endif
    }
};

#ifdef ENABLE_PROFILING
class ScopeDurationProfiler {
    std::string name;
    std::string category;
    std::chrono::high_resolution_clock::time_point start;
public:
    ScopeDurationProfiler(std::string_view name) noexcept;
    ScopeDurationProfiler(std::string_view name, std::string_view category) noexcept;
    ~ScopeDurationProfiler();
};
#endif

#ifdef ENABLE_PROFILING
#define PROFILE_SCOPE(name, category) ScopeDurationProfiler scope_profiler_super_ugly_name___(name, category);
#define PROFILE_INSTANT_GLOBAL(name, category) Profiler::instance().push_event(std::make_unique<Profiler::InstantEvent>(name, category, Profiler::InstantEvent::Scope::Global));
#define PROFILE_INSTANT_THREAD(name, category) Profiler::instance().push_event(std::make_unique<Profiler::InstantEvent>(name, category, Profiler::InstantEvent::Scope::Thread));
#define PROFILE_INSTANT_PROCESS(name, category) Profiler::instance().push_event(std::make_unique<Profiler::InstantEvent>(name, category, Profiler::InstantEvent::Scope::Process));
#else
#define PROFILE_SCOPE(name, category)
#define PROFILE_INSTANT_GLOBAL(name, category) 
#define PROFILE_INSTANT_THREAD(name, category) 
#define PROFILE_INSTANT_PROCESS(name, category) 
#endif

#endif