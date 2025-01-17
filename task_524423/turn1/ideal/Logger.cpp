#include "Logger.h"

Logger::Logger(size_t bufferSize) : bufferSize(bufferSize), logBuffer(bufferSize) {}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(bufferMutex);

    auto now = std::chrono::system_clock::now();
    logBuffer[writeIndex] = {level, message, now};
    writeIndex = (writeIndex + 1) % bufferSize;

    if (writeIndex == 0) {
        isBufferFull = true;
    }
}

std::vector<LogEntry> Logger::retrieveLogs(LogLevel minLevel) {
    std::lock_guard<std::mutex> lock(bufferMutex);
    std::vector<LogEntry> result;

    size_t start = isBufferFull ? writeIndex : 0;
    size_t count = isBufferFull ? bufferSize : writeIndex;

    for (size_t i = 0; i < count; ++i) {
        size_t index = (start + i) % bufferSize;
        if (logBuffer[index].level <= minLevel) {
            result.push_back(logBuffer[index]);
        }
    }

    return result;
}

void Logger::persistLogs() {
    std::cout << "Persisting Logs:\n";
    for (const auto& log : retrieveLogs(LogLevel::INFO)) {
        std::cout << formatTimestamp(log.timestamp) << " [" << static_cast<int>(log.level)
                  << "] " << log.message << "\n";
    }
}

std::string Logger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
