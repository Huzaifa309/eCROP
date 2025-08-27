#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include "Message.h"
#include "Types.h"

class MessageFlow final {
   private:
    using Step = std::function<StepResult(const Message&)>;

    static std::unordered_map<MessageType, std::vector<Step>> _registry;

   private:
    MessageFlow() noexcept = delete;
    ~MessageFlow() noexcept = delete;

    MessageFlow(const MessageFlow&) noexcept = delete;
    MessageFlow& operator=(const MessageFlow&) noexcept = delete;
    MessageFlow(MessageFlow&&) noexcept = delete;
    MessageFlow& operator=(MessageFlow&&) noexcept = delete;

   public:
    static void register_flow(MessageType msgType,
                              std::vector<Step> steps) noexcept;

    // Initialize MessageFlow
    static void initialize() noexcept;

    static void execute(const Message& msg) noexcept;
};
