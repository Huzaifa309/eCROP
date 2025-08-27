#include "MessageFlow.h"

#include "loggerlib.h"

// Static member definition
std::unordered_map<MessageType, std::vector<MessageFlow::Step>>
    MessageFlow::_registry;

void MessageFlow::register_flow(MessageType msgType,
                                std::vector<Step> steps) noexcept {
    _registry[msgType] = std::move(steps);
}

void MessageFlow::initialize() noexcept {
    // Register Order flow
    register_flow(  //
        MT_ORDER,   //
        {
            [](const Message& m) {
                return static_cast<const OrderMessage&>(m).validate();
            },
            //[&](const Message& m) { return risk.check(m); },
            //[&](const Message& m) { return router.send(m); },
        });

    // Register Cancel flow
    register_flow(  //
        MT_CANCEL,  //
        {
            [](const Message& m) {
                return static_cast<const CancelMessage&>(m).validate();
            },
            //[&](const Message& m) { return risk.check(m); },
            //[&](const Message& m) { return router.send(m); }
        });
}

void MessageFlow::execute(const Message& msg) noexcept {
    auto itr = _registry.find(msg.msgType);

    if (itr == _registry.end()) {
        qLogger::get().error_fast(
            "No flow registered for message type {} and message id {}",
            msgtype_to_string(msg.msgType), msg.msgId);
        return;
    }

    for (auto& step : itr->second) {
        StepResult res = step(msg);
        if (res == StepResult::FAILED) {
            qLogger::get().error_fast("Flow Msg {} {} stopped due to failure",
                                      msgtype_to_string(msg.msgType),
                                      msg.msgId);
            return;
        }
    }

    qLogger::get().info_fast("Flow Msg {} {} completed successfully",
                             msgtype_to_string(msg.msgType), msg.msgId);
}
