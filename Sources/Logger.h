#ifndef DEF_LOGGER_H
#define DEF_LOGGER_H

#include <iostream>
#include <cstdint>
#include <thread>
#include <fstream>
#include <vector>
#include <mutex>
#include <atomic>
#include <string>

enum class LogPriority : uint8_t {
    critical,
    warning,
    info
};

std::ostream& operator<<(std::ostream& out, LogPriority priority) noexcept;

class LogEntry {
public:
    using clock = std::chrono::system_clock;
private:
    clock::time_point time_point_;
    std::string message_;
    std::string category_;
    LogPriority priority_;
    std::thread::id thread_id_;
public:
    LogEntry(const std::string& category, LogPriority priority, const std::string& message, std::thread::id tid) noexcept;
    LogEntry(const std::string& category, LogPriority priority, const std::string& message, std::thread::id tid, clock::time_point tp) noexcept;

    const std::string& message() const noexcept;
    const std::string& category() const noexcept;
    LogPriority priority() const noexcept;
    clock::time_point time_point() const noexcept;
    std::thread::id thread_id() const noexcept;
};

std::ostream& operator<<(std::ostream& os, const LogEntry& entry) noexcept;

class Logger {
    std::vector<LogEntry> entries;
    std::mutex entries_mutex;
    std::thread background_thread;
    std::atomic_bool is_running;

    void background_work();
    Logger() noexcept;
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    ~Logger() noexcept;

    static Logger& instance();

    void start() noexcept;
    void stop() noexcept;
    void log(const LogEntry& entry);

    void log_warning(const std::string& category, const std::string& message) noexcept;
    void log_critical(const std::string& category, const std::string& message) noexcept;
    void log_info(const std::string& category, const std::string& message) noexcept;
};

void log_warning(const std::string& category, const std::string& message);
void log_critical(const std::string& category, const std::string& message);
void log_info(const std::string& category, const std::string& message);

#endif