// Cpp Standard Header Lib
#include <atomic>
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

// Local Headers include
#include "Config.h"
#include "DatabaseFactory.h"
#include "MessageFlow.h"
#include "eCropEngine.h"
#include "loggerlib.h"

int main(int argc, char** argv) {
    qLogger::get().initialize("logs/ecrop_engine.log", LogLevel::DEBUG);

    // Initialize the factory with default database types
    DatabaseFactory::initialize();
    // Initialize the message flow
    MessageFlow::initialize();

    std::atomic<bool> keepRunning{true};
    // Start input monitoring thread
    std::thread inputThread([&keepRunning]() {
        std::cin.get();  // Wait for Enter key
        keepRunning = false;
    });

    try {
        auto eKYC = std::make_unique<eCropEngine>();

        eKYC->start();

        while (keepRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        if (inputThread.joinable()) inputThread.join();

        eKYC->stop();
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        qLogger::get().error_fast("Error: {}", e.what());
        return EXIT_FAILURE;
    }
}
