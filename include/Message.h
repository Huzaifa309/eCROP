#pragma once

#include <string>

#include "Types.h"

class Message {
   public:
    Message(MessageType mType, int mId) noexcept;

    virtual ~Message() noexcept = default;

   public:
    MessageType msgType;
    int msgId;
};

class OrderMessage final : public Message {
   public:
    OrderMessage(int msgId, std::string sym, int qty, double prc) noexcept;

    ~OrderMessage() noexcept = default;

    StepResult validate() const noexcept;

   public:
    std::string symbol;
    int quantity;
    double price;
};

class CancelMessage final : public Message {
   public:
    CancelMessage(int msgId, int cId) noexcept;

    ~CancelMessage() noexcept = default;

    StepResult validate() const noexcept;

   public:
    int cancelId;
};
