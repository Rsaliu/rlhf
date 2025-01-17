#ifndef LOGGER_H
#define LOGGER_H

#include <array>
#include <string>
#include <mutex>
#include <chrono>
#include <thread>
#include <queue>
#include <condition_variable>
#include <iostream>
#include <sstream>

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
    explicit Logger(size_t bufferSize, const std::string& mqttBroker, int mqttPort = 1883);
    ~Logger();

    void log(LogLevel level, const std::string& message);
    std::vector<LogEntry> retrieveLogs(LogLevel minLevel = LogLevel::INFO);

private:
    size_t bufferSize;
    std::vector<LogEntry> logBuffer;
    size_t writeIndex = 0;
    bool isBufferFull = false;
    std::mutex bufferMutex;

    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;

    std::thread transmissionThread;
    std::queue<LogEntry> remoteLogs;
    std::mutex remoteMutex;
    std::condition_variable remoteCV;
    bool stopTransmission = false;

    void transmitLogs();
    void connectMQTT(const std::string& broker, int port);
    void publishLog(const LogEntry& log);

    // MQTT-related functions
    class MqttClient {
    public:
        MqttClient() = default;
        ~MqttClient() = default;
        bool connect(const std::string& broker, int port);
        bool publish(const std::string& topic, const std::string& message);
        void disconnect();

    private:
        // Placeholder for MQTT client implementation
    };

    MqttClient mqttClient;
    const std::string mqttBroker;
    int mqttPort;
};

#endif // LOGGER_H