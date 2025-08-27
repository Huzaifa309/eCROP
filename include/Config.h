#pragma once
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "helper.h"

class Config final {
   public:
    // Logging
    std::string LOG_DIR;
    size_t ROTATIING_LOG_SIZE;

    // Sharding
    int NUM_SHARDS;
    size_t MAX_RING_BUFFER_SIZE;

    // Aeron
    std::string AERON_DIR;
    std::string AERON_PROTOCOL;
    std::string SUBSCRIPTION_IP;
    int SUBSCRIPTION_PORT;
    std::string PUBLICATION_IP;
    int PUBLICATION_PORT;
    int SUBSCRIPTION_STREAM_ID;
    int PUBLICATION_STREAM_ID;

    // Database
    std::string DB_HOST;
    int DB_PORT;
    std::string DB_NAME;
    std::string DB_USER;
    std::string DB_PASSWORD;

    // Performance tuning
    int SHARD_TIMEOUT_MS;
    int IDLE_STRATEGY_SPINS;
    int IDLE_STRATEGY_YIELDS;

    static Config& get() {
        static Config config;
        return config;
    }

   private:
    Config() {
        load("../config.txt");  // Change to your path
    }

    ~Config() noexcept = default;

    Config(const Config&) noexcept = delete;
    Config& operator=(const Config&) noexcept = delete;
    Config(Config&&) noexcept = delete;
    Config& operator=(Config&&) noexcept = delete;

    void load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Could not open config file: " + filename);

        std::string line;
        while (std::getline(file, line)) {
            // Remove comments
            if (auto pos = line.find('#'); pos != std::string::npos)
                line.erase(pos);

            // Trim spaces
            trim(line);
            if (line.empty()) continue;

            // Split key=value
            auto delimPos = line.find('=');
            if (delimPos == std::string::npos) continue;

            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 1);
            trim(key);
            trim(value);

            assign_value(key, value);
        }
    }

    void assign_value(const std::string& key, const std::string& value) {
        if (key == "LOG_DIR")
            LOG_DIR = value;
        else if (key == "ROTATIING_LOG_SIZE")
            ROTATIING_LOG_SIZE = std::stoull(value);
        else if (key == "NUM_SHARDS")
            NUM_SHARDS = std::stoi(value);
        else if (key == "MAX_RING_BUFFER_SIZE")
            MAX_RING_BUFFER_SIZE = std::stoull(value);
        else if (key == "AERON_DIR")
            AERON_DIR = value;
        else if (key == "AERON_PROTOCOL")
            AERON_PROTOCOL = value;
        else if (key == "SUBSCRIPTION_IP")
            SUBSCRIPTION_IP = value;
        else if (key == "SUBSCRIPTION_PORT")
            SUBSCRIPTION_PORT = std::stoi(value);
        else if (key == "PUBLICATION_IP")
            PUBLICATION_IP = value;
        else if (key == "PUBLICATION_PORT")
            PUBLICATION_PORT = std::stoi(value);
        else if (key == "SUBSCRIPTION_STREAM_ID")
            SUBSCRIPTION_STREAM_ID = std::stoi(value);
        else if (key == "PUBLICATION_STREAM_ID")
            PUBLICATION_STREAM_ID = std::stoi(value);
        else if (key == "DB_HOST")
            DB_HOST = value;
        else if (key == "DB_PORT")
            DB_PORT = std::stoi(value);
        else if (key == "DB_NAME")
            DB_NAME = value;
        else if (key == "DB_USER")
            DB_USER = value;
        else if (key == "DB_PASSWORD")
            DB_PASSWORD = value;
        else if (key == "SHARD_TIMEOUT_MS")
            SHARD_TIMEOUT_MS = std::stoi(value);
        else if (key == "IDLE_STRATEGY_SPINS")
            IDLE_STRATEGY_SPINS = std::stoi(value);
        else if (key == "IDLE_STRATEGY_YIELDS")
            IDLE_STRATEGY_YIELDS = std::stoi(value);
    }
};
