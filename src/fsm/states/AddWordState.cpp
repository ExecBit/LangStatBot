#include "AddWordState.h"
#include "IdleState.h"
#include "logger/Logger.h"
#include "fsm/StateMachine.h"

namespace fsm {
AddWordState::AddWordState(const core::Message& message) : initMessage(message) {}

void AddWordState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    dialog.context.bot->sendMessage(initMessage.chat_id, "Enter text", def::KeyboardType::none);
    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onMessage(stateMachine, message);
    };
}

void AddWordState::onMessage(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onMessage", _name);

    SPDLOG_INFO("Add word: {}", message.text);
    dialog.context.data.stat->words.push_back(message.text);
    dialog.context.bot->sendMessage(message.chat_id, "ok", def::KeyboardType::keyboardWithLayout);

    dialog.setState<IdleState>();
}
}; // namespace fsm
