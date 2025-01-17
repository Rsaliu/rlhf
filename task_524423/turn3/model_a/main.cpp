#include "Logger.h"
#include <thread>

void generateLogs(Logger& logger) {
    logger.log(LogLevel::INFO, "UID_001", "ModuleA", "System initialized.", 1);
    logger.log(LogLevel::WARNING, "UID_002", "ModuleA", "High temperature detected.", 2);
    logger.log(LogLevel::ERROR, "UID_003", "ModuleB", "Sensor failure.", 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    logger.log(LogLevel::INFO, "UID_004", "ModuleA", "System rebooted.", 1);
}

int main() {
    Logger logger(10, 20);
    logger.setRemoteEnabled(true); // Enable remote logging

    std::thread logThread1(generateLogs, std::ref(logger));
    std::thread logThread2(generateLogs, std::ref(logger));

    logThread1.join();
    logThread2.join();

    logger.persistLogs();

    return 0;
}