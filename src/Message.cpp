#include "Message.h"

#include "loggerlib.h"

Message::Message(MessageType mType, int mId) noexcept
    : msgType(mType), msgId(mId) {}

OrderMessage::OrderMessage(int msgId, std::string sym, int qty,
                           double prc) noexcept
    : Message(MT_ORDER, msgId),
      symbol(std::move(sym)),
      quantity(qty),
      price(prc) {}

StepResult OrderMessage::validate() const noexcept {
    qLogger::get().info_fast("Order {} Validate symbol={}", msgId, symbol);

    if (quantity <= 0 || price <= 0.0) {
        qLogger::get().error_fast("Order {} Invalid quantity/price", msgId);
        return StepResult::FAILED;
    }
    return StepResult::SUCCESS;
}

CancelMessage::CancelMessage(int msgId, int cId) noexcept
    : Message(MT_CANCEL, msgId), cancelId(cId) {}

StepResult CancelMessage::validate() const noexcept {
    qLogger::get().info_fast("Cancel {} Validate cancelId= {}", msgId,
                             cancelId);

    if (cancelId <= 0) {
        qLogger::get().error_fast("Cancel {} Invalid cancelId", msgId);
        return StepResult::FAILED;
    }
    return StepResult::SUCCESS;
}
