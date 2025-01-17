#include <iostream>
#include <string>
#include <array>
#include <algorithm>

enum class LogLevel {
    ERROR = 0,
    WARNING = 1,
    INFO = 2
};

class CircularLogBuffer {
public:
    CircularLogBuffer(size_t capacity) : buffer(capacity), startIndex(0), nextIndex(0), capacity(capacity) {}

    void log(LogLevel level, const std::string& message) {
        if (nextIndex == startIndex) {
            // Buffer is full, overwrite the oldest log
            startIndex = (startIndex + 1) % capacity;
        }

        buffer[nextIndex] = {level, message};
        nextIndex = (nextIndex + 1) % capacity;
    }

    const std::vector<std::pair<LogLevel, std::string>>& getLogs() const {
        std::vector<std::pair<LogLevel, std::string>> logs(buffer.begin(), buffer.end());

        // Trim logs from startIndex to the end of the buffer
        if (startIndex < nextIndex) {
            logs.erase(logs.begin(), logs.begin() + startIndex);
        } else {
            // Wrapped around, trim logs from start of buffer to end
            logs.erase(logs.begin(), logs.begin() + nextIndex);
        }

        return logs;
    }

private:
    std::array<std::pair<LogLevel, std::string>, 100> buffer; // Replace with the desired capacity
    size_t startIndex;
    size_t nextIndex;
    size_t capacity;
};

void persistLogs(const CircularLogBuffer& logBuffer) {
    // TODO: Implement log persistence logic, e.g., save to SD card, flash memory, or send over network
    for (const auto& log : logBuffer.getLogs()) {
        std::cout << "[";
        switch (log.first) {
            case LogLevel::ERROR:
                std::cout << "ERROR";
                break;
            case LogLevel::WARNING:
                std::cout << "WARNING";
                break;
            case LogLevel::INFO:
                std::cout << "INFO";
                break;
        }
        std::cout << "]: " << log.second << std::endl;
    }
}

int main() {
    CircularLogBuffer logBuffer(10); // Adjust capacity as needed

    logBuffer.log(LogLevel::ERROR, "System overheated");
    logBuffer.log(LogLevel::INFO, "Sensor data updated");
    logBuffer.log(LogLevel::WARNING, "Communication link unstable");

    // Simulate periodic logging
    for (int i = 0; i < 10; i++) {
        logBuffer.log(LogLevel::INFO, "Regular log message " + std::to_string(i));
    }

    // Retrieve and persist logs
    persistLogs(logBuffer);

    return 0;
}