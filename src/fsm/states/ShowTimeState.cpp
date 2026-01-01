#include "ShowTimeState.h"

#include "Data.h"
#include "IdleState.h"
#include "def.h"
#include "fsm/StateMachine.h"
#include "logger/Logger.h"

#include <string>

namespace fsm {
ShowTimeState::ShowTimeState(const core::Message& message) : initMessage(message) {}

void ShowTimeState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    if (!dialog.context.data.stat) {
        dialog.context.bot->sendMessage(initMessage.chat_id, "Sheduler is empty",
                                        def::KeyboardType::keyboardWithLayout);
        dialog.setState<IdleState>();
        return;
    }

    dialog.context.bot->sendMessage(initMessage.chat_id, "Enter month number",
                                    def::KeyboardType::keyboardChooseMonth);
    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingMonthNumber(stateMachine, message);
    };
}

void ShowTimeState::onWaitingMonthNumber(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingMonthNumber", _name);
    SPDLOG_INFO("Got Month number {}", message.text);

    auto& dialogContext = dialog.context;

    if (const auto res = def::toInt(message.text); !res) {
        SPDLOG_WARN("Wrong value - {}", message.text);
        dialog.setState<IdleState>();
        return;
    } else {
        const auto& monthStat = dialogContext.data.stat->years[2025][*res];
        dialogContext.bot->sendMessage(message.chat_id, monthStat.print(),
                                       def::KeyboardType::keyboardWithLayout);
    }

    dialog.setState<IdleState>();
}
}; // namespace fsm
