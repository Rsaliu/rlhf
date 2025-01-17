#include "Logger.h"

Logger::Logger(size_t bufferSize, size_t remoteQueueSize) 
: bufferSize(bufferSize), logBuffer(bufferSize) {
    remoteThread = std::thread(&Logger::remoteLogHandler, this);
}

Logger::~Logger() {
    stopThread = true;
    queueCondVar.notify_all();
    if (remoteThread.joinable()) {
        remoteThread.join();
    }
}

void Logger::log(LogLevel level, const std::string& uniqueID, const std::string& moduleID, const std::string& message, int severity) {
    std::lock_guard<std::mutex> lock(bufferMutex);

    auto now = std::chrono::system_clock::now();
    logBuffer[writeIndex] = {level, message, moduleID, uniqueID, now, severity};
    writeIndex = (writeIndex + 1) % bufferSize;

    if (writeIndex == 0) {
        isBufferFull = true;
    }

    // If remote logging is enabled, add the log to the queue
    if (remoteEnabled) {
        std::lock_guard<std::mutex> queueLock(queueMutex);
        remoteQueue.push(logBuffer[writeIndex]);
        queueCondVar.notify_one();
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
        std::cout << formatTimestamp(log.timestamp) 
                  << " [" << static_cast<int>(log.level) << "] "
                  << "Module: " << log.moduleID << " "
                  << "Unique ID: " << log.uniqueID << " "
                  << "Severity: " << log.severity << " "
                  << log.message << "\n";
    }
}

void Logger::remoteLogHandler() {
    while (!stopThread) {
        std::unique_lock<std::mutex> queueLock(queueMutex);
        queueCondVar.wait(queueLock, [this] { return !remoteQueue.empty() || stopThread; });

        while (!remoteQueue.empty()) {
            auto log = remoteQueue.front();
            remoteQueue.pop();
            queueLock.unlock();
            sendLog(log);
            queueLock.lock();
        }
    }
}

void Logger::sendLog(const LogEntry& log) {
    // Simulate sending a log over the network (replace with actual implementation)
    std::cout << "Sending Log: " << formatTimestamp(log.timestamp) 
              << " [" << static_cast<int>(log.level) << "] "
              << "Module: " << log.moduleID << " "
              << "Unique ID: " << log.uniqueID << " "
              << "Severity: " << log.severity << " "
              << log.message << "\n";
    
    // Actual implementation would send the log data over the network
}

std::string Logger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}