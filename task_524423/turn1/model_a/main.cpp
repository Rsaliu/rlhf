
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
#include <ctime>
#include <mutex>
enum class LogLevel {
    ERROR,
    WARNING,
    INFO
};
#define BUFFER_SIZE 1024  // Size of the circular buffer

struct LogEntry {
    LogLevel level;
    std::string message;
    std::time_t timestamp;

    LogEntry(LogLevel lvl, const std::string& msg)
        : level(lvl), message(msg), timestamp(std::time(nullptr)) {}
};

class CircularBuffer {
private:
    LogEntry* buffer;
    size_t head;
    size_t size;
    size_t count;
    std::mutex mtx;

public:
    CircularBuffer(size_t bufferSize) : head(0), size(bufferSize), count(0) {
        buffer = new LogEntry[size];
    }

    ~CircularBuffer() {
        delete[] buffer;
    }

    void log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(mtx);
        buffer[head] = LogEntry(level, message); // write log entry
        head = (head + 1) % size;
        if (count < size) {
            count++;
        }
    }

    void retrieveLogs(std::ostringstream& oss) {
        std::lock_guard<std::mutex> lock(mtx);
        size_t idx = (head >= count) ? head - count : (head + size - count);

        for (size_t i = 0; i < count; i++) {
            LogEntry& entry = buffer[(idx + i) % size];
            oss << std::ctime(&entry.timestamp);
            oss << " [" << static_cast<int>(entry.level) << "] " << entry.message << "\n";
        }
    }
};
class Logger {
private:
    CircularBuffer& buffer;

public:
    Logger(CircularBuffer& circBuffer) : buffer(circBuffer) {}

    void logError(const std::string& message) {
        buffer.log(LogLevel::ERROR, message);
    }

    void logWarning(const std::string& message) {
        buffer.log(LogLevel::WARNING, message);
    }

    void logInfo(const std::string& message) {
        buffer.log(LogLevel::INFO, message);
    }

    void retrieveLogs(std::ostringstream& oss) {
        buffer.retrieveLogs(oss);
    }
};
void persistLogs(CircularBuffer& buffer) {
    std::ostringstream oss;
    buffer.retrieveLogs(oss);
    
    std::ofstream logFile("logs.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << oss.str();
        logFile.close();
    }
}

int main() {
    CircularBuffer circularBuffer(BUFFER_SIZE);
    Logger logger(circularBuffer);
    
    logger.logInfo("System started");
    logger.logWarning("Low memory warning");
    logger.logError("Failed to connect to sensor");

    // Simulate persisting logs (ideally done in a separate thread)
    persistLogs(circularBuffer);

    return 0;
}