#pragma once

#include "def.h"

namespace fsm {
class StateMachine;
};

namespace core {

class IBot {
  public:
    virtual ~IBot() = default;
    virtual void sendMessage(std::int64_t chatId, const std::string& text, def::KeyboardType) = 0;
    virtual void sendDocument(std::int64_t chatId, const def::DocumentData& doc,
                              def::KeyboardType) = 0;
    virtual void deleteWebhook() = 0;
    virtual void deleteMyCommands() = 0;
    virtual std::string username() = 0;
    virtual void setCommands() = 0;
    virtual void setCallback(fsm::StateMachine& stateMachine) = 0;
    virtual void startLongPoll() = 0;
};

}; // namespace core
