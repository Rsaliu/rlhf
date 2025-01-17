#include "Logger.h"
#include <thread>
#include <queue>
#include <condition_variable>
#include <iostream>

Logger::Logger(size_t bufferSize, const std::string& mqttBroker, int mqttPort)
    : bufferSize(bufferSize), logBuffer(bufferSize), mqttBroker(mqttBroker), mqttPort(mqttPort)
{
    transmissionThread = std::thread(&Logger::transmitLogs, this);
}

Logger::~Logger() {
    {
        std::lock_guard<std::mutex> lock(remoteMutex);
        stopTransmission = true;
    }
    remoteCV.notify_one();
    transmissionThread.join();
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(bufferMutex);

    auto now = std::chrono::system_clock::now();
    logBuffer[writeIndex] = {level, message, now};
    writeIndex = (writeIndex + 1) % bufferSize;

    if (writeIndex == 0) {
        isBufferFull = true;
    }

    {
        std::lock_guard<std::mutex> lock(remoteMutex);
        remoteLogs.push(logBuffer[writeIndex]);
    }
    remoteCV.notify_one();
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

void Logger::transmitLogs() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(remoteMutex);
            remoteCV.wait(lock, [this] { return stopTransmission || !remoteLogs.empty(); });
            if (stopTransmission) {
                break;
            }

            while (!remoteLogs.empty()) {
                LogEntry log = remoteLogs.front();
                remoteLogs.pop();
                publishLog(log);
            }
        }
    }
}

void Logger::publishLog(const LogEntry& log) {
    // Simulate MQTT publish
    std::stringstream ss;
    ss << formatTimestamp(log.timestamp) << " [" << static_cast<int>(log.level) << "] " << log.message;
    std::cout << "Publishing log: " << ss.str() << std::endl;

    // Implement actual MQTT publish using mqttClient
    // mqttClient.publish("iot/device/logs", ss.str());
}

void Logger::connectMQTT(const std::string& broker, int port) {
    // Simulate MQTT connection
    std::cout << "Connecting to MQTT broker: " << broker << ":" << port << std::endl;

    // Implement actual MQTT connection using mqttClient
    // bool success = mqttClient.connect(broker, port);
    // if (!success) {
    //     std::cerr << "Failed to connect to MQTT broker." << std::endl;
    // }
}