#ifndef LOGGER_H
#define LOGGER_H

#include <array>
#include <string>
#include <mutex>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <vector>

enum class LogLevel {
    ERROR,
    WARNING,
    INFO,
    DEBUG // Additional level for more granularity
};

struct LogEntry {
    LogLevel level;
    std::string message;
    std::string moduleID;   // Identifier for the module
    std::string uniqueID;    // Unique identifier for the log entry
    std::chrono::system_clock::time_point timestamp;
    int severity;            // Additional severity level (e.g., 1-10)
};

class Logger {
public:
    explicit Logger(size_t bufferSize, size_t remoteQueueSize);
    ~Logger();

    void log(LogLevel level, const std::string& uniqueID, const std::string& moduleID, const std::string& message, int severity = 0);
    std::vector<LogEntry> retrieveLogs(LogLevel minLevel = LogLevel::INFO);
    void persistLogs();
    void setRemoteEnabled(bool enabled) { remoteEnabled = enabled; }

private:
    size_t bufferSize;
    std::vector<LogEntry> logBuffer;
    size_t writeIndex = 0;
    bool isBufferFull = false;

    std::mutex bufferMutex;
    std::queue<LogEntry> remoteQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;
    std::thread remoteThread;
    std::atomic<bool> remoteEnabled{false};
    bool stopThread = false;

    void remoteLogHandler();
    void sendLog(const LogEntry& log);
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
};

#endif // LOGGER_H