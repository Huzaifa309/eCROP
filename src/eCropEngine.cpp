#include "eCropEngine.h"

#include <exception>

#include "Config.h"
#include "loggerlib.h"

eCropEngine::eCropEngine() noexcept
    : _running(false), _requestReceived(0), _requestHandler() {
    try {
        auto &cfg = Config::get();
        _aeron = std::make_unique<aeron_wrapper::Aeron>(cfg.AERON_DIR);
        qLogger::get().info_fast("Connected to Aeron Media Driver...");
        std::string subscriptionChannel =
            "aeron:" + cfg.AERON_PROTOCOL + "?endpoint=" + cfg.SUBSCRIPTION_IP +
            ":" + std::to_string(cfg.SUBSCRIPTION_PORT);
        _subscription = _aeron->create_subscription(subscriptionChannel,  //
                                                    cfg.SUBSCRIPTION_STREAM_ID);
        std::string publicationChannel =
            "aeron:" + cfg.AERON_PROTOCOL + "?endpoint=" + cfg.PUBLICATION_IP +
            ":" + std::to_string(cfg.PUBLICATION_PORT);
        _publication = _aeron->create_publication(publicationChannel,  //
                                                  cfg.PUBLICATION_STREAM_ID);

        _running = true;
    } catch (const std::exception &e) {
        qLogger::get().info_fast("Error: {}", e.what());
    }
}

eCropEngine::~eCropEngine() noexcept { stop(); }

void eCropEngine::start() noexcept {
    if (!_running) return;

    qLogger::get().info_fast("Starting eCROP engine...");
    // Start background msg processing
    _backgroundPoller = _subscription->start_background_polling(
        [this](const aeron_wrapper::FragmentData &fragmentData) {
            receive_request(fragmentData);
        });
}

void eCropEngine::stop() noexcept {
    if (!_running) return;

    if (_backgroundPoller) {
        _backgroundPoller->stop();
    }

    _running = false;
    qLogger::get().info_fast("eCROP engine stopped.");
}

void eCropEngine::receive_request(
    const aeron_wrapper::FragmentData &fragmentData) noexcept {
    ++_requestReceived;
    try {
        auto buffer = _requestHandler.respond(fragmentData);
        send_response(buffer);
    } catch (const std::exception &e) {
        qLogger::get().error_fast("Error: {}", e.what());
    }
}

void eCropEngine::send_response(std::vector<char> &buffer) noexcept {
    if (!_publication) return;

    if (buffer.empty()) return;

    auto result = _publication->offer(
        reinterpret_cast<const uint8_t *>(buffer.data()), buffer.size());
    if (result == aeron_wrapper::PublicationResult::SUCCESS) {
        qLogger::get().info_fast("Response sent successfully");
    } else {
        qLogger::get().error_fast("Failed to send response: {}",
                                  pubresult_to_string(result));
    }
}
