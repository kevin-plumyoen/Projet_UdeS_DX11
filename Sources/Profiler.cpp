#include "Profiler.h"

#include <iterator>
#include <algorithm>
#include <fstream>

static std::chrono::high_resolution_clock::time_point start_of_profiling = std::chrono::high_resolution_clock::now();

Profiler::Event::Event(std::string_view name) noexcept
: name{name}
, category{}
, ts{clock::now()}
, tid{std::this_thread::get_id()} {

}

Profiler::Event::Event(std::string_view name, std::string_view category) noexcept 
: name{name}
, category{category}
, ts{clock::now()}
, tid{std::this_thread::get_id()} {

}

Profiler::Event::Event(std::string_view name, std::string_view category, clock::time_point ts) noexcept
: name{name}
, category{category}
, ts{ts}
, tid{std::this_thread::get_id()} {

}

std::ostream& Profiler::Event::serialize(std::ostream& out) const {
    return out << "\"name\":\"" << name << "\","
               << "\"cat\":\"" << category << "\","
               << "\"ts\":" << std::chrono::duration_cast<std::chrono::microseconds>(ts - start_of_profiling).count() << ","
               << "\"tid\":" << tid << ","
               << "\"pid\": 0";
}

Profiler::DurationEvent::DurationEvent(std::string_view name, Type type) noexcept
: Event{name}
, type{type} {

}

Profiler::DurationEvent::DurationEvent(std::string_view name, std::string_view category, Type type) noexcept
: Event{name, category}
, type{type} {

}

std::ostream& Profiler::DurationEvent::serialize(std::ostream& out) const {
    return Event::serialize(out) << ","
           << "\"ph\":\"" << (type == Type::Start ? 'B' : 'E') << "\"";
}

Profiler::CompleteEvent::CompleteEvent(std::string_view name, clock::time_point start, std::chrono::microseconds duration) noexcept 
: Event{name, "", start}
, duration{duration} {

}

Profiler::CompleteEvent::CompleteEvent(std::string_view name, std::string_view category, clock::time_point start, std::chrono::microseconds duration) noexcept
: Event{name, category, start}
, duration{duration} {

}

std::ostream& Profiler::CompleteEvent::serialize(std::ostream& out) const {
    return Event::serialize(out) << ","
           << "\"ph\":\"X\","
           << "\"dur\":" << duration.count();
}

Profiler::InstantEvent::InstantEvent(std::string_view name) noexcept
: InstantEvent{name, Scope::Thread} {

}

Profiler::InstantEvent::InstantEvent(std::string_view name, std::string_view category) noexcept
: InstantEvent{name, category, Scope::Thread} {

}

Profiler::InstantEvent::InstantEvent(std::string_view name, Scope scope) noexcept 
: Event{name} 
, scope{scope} {

}

Profiler::InstantEvent::InstantEvent(std::string_view name, std::string_view category, Scope scope) noexcept 
: Event{name, category}
, scope{scope} {

}

constexpr char scope_to_char(const Profiler::InstantEvent::Scope& scope) noexcept {
    switch (scope) {
    case Profiler::InstantEvent::Scope::Process:
        return 'p';
    case Profiler::InstantEvent::Scope::Global:
        return 'g';
    case Profiler::InstantEvent::Scope::Thread:
        return 't';
    }

    return '?';
}

std::ostream& Profiler::InstantEvent::serialize(std::ostream& out) const {
    return Event::serialize(out) << ","
           << "\"ph\":\"i\","
           << "\"s\":\"" << scope_to_char(scope) << "\"";
}

std::ostream& Profiler::write_header(std::ostream& stream) {
    return stream << "{"
                  << "\"displayTimeUnit\":\"ns\","
                  << "\"traceEvents\": [";
}

std::ostream& Profiler::write_footer(std::ostream& stream) {
    return stream << "]"  // Close traceEvents
                  << "}"; // Close root bracket
}

#ifdef ENABLE_PROFILING
void Profiler::serialize_events_thread() {
    std::ofstream file("profile.json.tmp");
    write_header(file);

    std::vector<std::unique_ptr<Event>> event_to_write;
    while (is_running) {
        {
            std::lock_guard<std::mutex> lock(m);
            std::swap(event_to_write, events);
        }

        // Écrit les événements collectés jusqu'à présent dans le fichier
        std::for_each(event_to_write.begin(), event_to_write.end(), [&file](const std::unique_ptr<Event>& ev) {
            file << "{" << *ev << "},";
        });
        event_to_write.clear();

        std::this_thread::yield();
    }

    // Écrit les événements qui aurait pu être ajouté durant le yield
    {
        std::lock_guard<std::mutex> lock(m);
        std::for_each(events.begin(), events.end(), [&file](const std::unique_ptr<Event>& ev) {
            file << "{" << *ev << "},";
        });
    }

    file.flush();
    std::ifstream input_file("profile.json.tmp");
    std::vector<char> buffer(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>{});
    buffer.pop_back();

    file = std::ofstream("profile.json");
    std::copy(buffer.begin(), buffer.end(), 
              std::ostreambuf_iterator<char>(file));
    write_footer(file);
}
#endif

Profiler::Profiler()
#ifdef ENABLE_PROFILING
: is_running{true}
, background_thread{&Profiler::serialize_events_thread, this} 
#endif
{

}

Profiler::~Profiler() {
#ifdef ENABLE_PROFILING
    is_running.store(false);
    background_thread.join();
#endif
}

Profiler& Profiler::instance() {
    static Profiler instance_;

    return instance_;
}

void Profiler::push_event(std::unique_ptr<Event> ev) {
#ifdef ENABLE_PROFILING
    std::lock_guard<std::mutex> lock(m);

    events.push_back(std::move(ev));
#endif
}

#ifdef ENABLE_PROFILING
ScopeDurationProfiler::ScopeDurationProfiler(std::string_view name) noexcept
: name{name}
, category{}
, start{std::chrono::high_resolution_clock::now()} {

}

ScopeDurationProfiler::ScopeDurationProfiler(std::string_view name, std::string_view category) noexcept
: name{name} 
, category{category}
, start{std::chrono::high_resolution_clock::now()} {

}

ScopeDurationProfiler::~ScopeDurationProfiler() {
    const std::chrono::microseconds scope_duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);

    Profiler::instance().push_event(std::make_unique<Profiler::CompleteEvent>(name, category, start, scope_duration));
}

#endif