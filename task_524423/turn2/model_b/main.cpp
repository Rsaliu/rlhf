#include "Logger.h"
#include <thread>

void generateLogs(Logger& logger) {
    logger.log(LogLevel::INFO, "System initialized.");
    logger.log(LogLevel::WARNING, "High temperature detected.");
    logger.log(LogLevel::ERROR, "Sensor failure.");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    logger.log(LogLevel::INFO, "System rebooted.");
}

int main() {
    Logger logger(10, "your.mqtt.broker.here");

    std::thread logThread1(generateLogs, std::ref(logger));
    std::thread logThread2(generateLogs, std::ref(logger));

    logThread1.join();
    logThread2.join();

    return 0;
}