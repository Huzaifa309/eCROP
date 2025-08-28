#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "MessageHandler.h"
#include "aeron_wrapper.h"

class eCROPEngine final {
   public:
    eCROPEngine() noexcept;

    ~eCROPEngine() noexcept;

    void start() noexcept;

    void stop() noexcept;

   private:
    void process_message(
        const aeron_wrapper::FragmentData &fragmentData) noexcept;
    void send_response(std::vector<char> &buffer) noexcept;

    // Aeron components
    std::unique_ptr<aeron_wrapper::Aeron> _aeron;
    std::unique_ptr<aeron_wrapper::Subscription> _subscription;
    std::unique_ptr<aeron_wrapper::Publication> _publication;
    std::unique_ptr<aeron_wrapper::Subscription::BackgroundPoller>
        _backgroundPoller;

    std::atomic<bool> _running;
    std::uint64_t _packetsReceived;

    MessageHandler _messageHandler;
};
