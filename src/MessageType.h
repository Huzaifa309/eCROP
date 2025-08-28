#pragma once

#include <cstdint>
#include <string>

enum class StepResult { SUCCESS, FAILED };

enum MessageType : std::int8_t {
    MT_ORDER,
    MT_CANCEL,
};

// Get publication constants as string for debugging
inline std::string msgtype_to_string(MessageType msgType) noexcept {
    switch (msgType) {
        case MessageType::MT_ORDER:
            return "MT_ORDER";
        case MessageType::MT_CANCEL:
            return "MT_CANCEL";
        default:
            return "UNKNOWN";
    }
}
