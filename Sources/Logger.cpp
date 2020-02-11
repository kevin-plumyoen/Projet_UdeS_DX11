#include "Logger.h"

#include <iterator>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <time.h>

std::ostream& operator<<(std::ostream& os, LogPriority priority) noexcept {
    switch(priority) {
        case LogPriority::critical:
            os << "crit";
            break;
        case LogPriority::info:
            os << "info";
            break;
        case LogPriority::warning:
            os << "warn";
            break;
    }
    return os;
}

LogEntry::LogEntry(const std::string& category, LogPriority priority, const std::string& message, std::thread::id tid) noexcept
: time_point_(clock::now())
, message_(message)
, category_(category)
, priority_{priority}
, thread_id_{tid} {

}

LogEntry::LogEntry(const std::string& category, LogPriority priority, const std::string& message, std::thread::id tid, clock::time_point tp) noexcept
: time_point_(tp)
, message_(message)
, category_(category)
, priority_{priority}
, thread_id_{tid} {

}

const std::string& LogEntry::message() const noexcept {
    return message_;
}

const std::string& LogEntry::category() const noexcept {
    return category_;
}

LogPriority LogEntry::priority() const noexcept {
    return priority_;
}

std::chrono::system_clock::time_point LogEntry::time_point() const noexcept {
    return time_point_;
}

std::thread::id LogEntry::thread_id() const noexcept {
    return thread_id_;
}

std::ostream& operator<<(std::ostream& os, const LogEntry& entry) noexcept {
    char time_buffer[128];
    std::fill(time_buffer, time_buffer + 128, static_cast<char>(0));

    const time_t t = LogEntry::clock::to_time_t(entry.time_point());
    std::tm temp;
    localtime_s(&temp, &t);
    strftime(time_buffer, 128, "[%H:%M:%S %z]", &temp);

    return os << entry.priority() << " "
              << entry.category() << " "
              << entry.thread_id() << " "
              << time_buffer
              << "(" << entry.time_point().time_since_epoch().count() << ") "
              << " \"" << entry.message() << "\"";
}

Logger::Logger() noexcept
: entries{}
, entries_mutex{}
, background_thread{}
, is_running{false} {
    entries.reserve(2048);
}

Logger::~Logger() noexcept {
    stop();
}

Logger& Logger::instance() {
    static Logger instance;

    return instance;
}

void Logger::start() noexcept {
    is_running = true;
    background_thread = std::thread(&Logger::background_work, std::ref(*this));
}

void Logger::stop() noexcept {
    is_running.store(false);

    if(background_thread.joinable()) {
        background_thread.join();
    }
}

void Logger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(entries_mutex);

    entries.push_back(entry);
}

void Logger::log_warning(const std::string& category, const std::string& message) noexcept {
    const LogEntry::clock::time_point tp = LogEntry::clock::now();

    {
        std::lock_guard<std::mutex> lock(entries_mutex);
        entries.emplace_back(category, LogPriority::warning, message, std::this_thread::get_id(), tp);
    }
}

void Logger::log_critical(const std::string& category, const std::string& message) noexcept {
    const LogEntry::clock::time_point tp = LogEntry::clock::now();

    {
        std::lock_guard<std::mutex> lock(entries_mutex);
        entries.emplace_back(category, LogPriority::critical, message, std::this_thread::get_id(), tp);
    }
}

void Logger::log_info(const std::string& category, const std::string& message) noexcept {
    const LogEntry::clock::time_point tp = LogEntry::clock::now();

    {
        std::lock_guard<std::mutex> lock(entries_mutex);
        entries.emplace_back(category, LogPriority::info, message, std::this_thread::get_id(), tp);
    }
}

void Logger::background_work() {
    std::ofstream out_log("latest_session.log", std::ios::binary);

    out_log << "# priority category tid [time](epoch) \"message\"" << std::endl;

    std::vector<LogEntry> write_entries;
    write_entries.reserve(2048);

    while(is_running) {
        {
            std::lock_guard<std::mutex> lock(entries_mutex);
            entries.swap(write_entries);
        }

        // Dump all write entries
        std::copy(std::begin(write_entries), std::end(write_entries), std::ostream_iterator<LogEntry>(out_log, "\n"));
        write_entries.clear();
        out_log.flush();

        std::this_thread::yield();
    }

    // Dump every entries
    std::copy(std::begin(entries), std::end(entries), std::ostream_iterator<LogEntry>(out_log));
    out_log.flush();
}

void log_warning(const std::string& category, const std::string& message) {
    Logger::instance().log_warning(category, message);
}

void log_critical(const std::string& category, const std::string& message) {
    Logger::instance().log_critical(category, message);
}

void log_info(const std::string& category, const std::string& message) {
    Logger::instance().log_info(category, message);
}