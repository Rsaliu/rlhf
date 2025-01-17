#ifndef LOGGER_H
#define LOGGER_H

#include <array>
#include <string>
#include <mutex>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

enum class LogLevel {
    ERROR,
    WARNING,
    INFO
};

struct LogEntry {
    LogLevel level;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
};

class Logger {
public:
    explicit Logger(size_t bufferSize);

    void log(LogLevel level, const std::string& message);
    std::vector<LogEntry> retrieveLogs(LogLevel minLevel = LogLevel::INFO);
    void persistLogs(); // Simulates persisting logs to non-volatile storage

private:
    size_t bufferSize;
    std::vector<LogEntry> logBuffer;
    size_t writeIndex = 0;
    bool isBufferFull = false;
    std::mutex bufferMutex;

    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
};

#endif // LOGGER_H
