#include "EditTimeState.h"

#include "IdleState.h"
#include "common/def.h"
#include "fsm/StateMachine.h"
#include "logger/Logger.h"

namespace fsm {
EditTimeState::EditTimeState(const core::Message& message) : initMessage(message) {}

void EditTimeState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    dialog.context.bot->sendMessage(initMessage.chat_id, "Enter year number",
                                    def::KeyboardType::keyboardChooseYear);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingYearNumber(stateMachine, message);
    };
}

void EditTimeState::onWaitingYearNumber(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingYearNumber", _name);

    if (const auto res = def::toInt(message.text); !res) {
        SPDLOG_WARN("Wrong value - {}", message.text);
        dialog.context.bot->sendMessage(message.chat_id, "Wrong value",
                                        def::KeyboardType::keyboardChooseCommands);
        dialog.setState<IdleState>();
        return;
    } else {
        dialog.context.yearNumber = *res;
    }

    SPDLOG_INFO("Add year number: {}", message.text);
    dialog.context.bot->sendMessage(message.chat_id, "Enter month number",
                                    def::KeyboardType::keyboardChooseMonth);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingMonthNumber(stateMachine, message);
    };
}

void EditTimeState::onWaitingMonthNumber(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingMonthNumber", _name);

    if (const auto res = def::toInt(message.text); !res) {
        SPDLOG_WARN("Wrong value - {}", message.text);
        dialog.context.bot->sendMessage(message.chat_id, "Wrong value",
                                        def::KeyboardType::keyboardChooseCommands);
        dialog.setState<IdleState>();
        return;
    } else {
        SPDLOG_INFO("Add month number: {}", message.text);
        dialog.context.monthNumber = *res;
    }

    dialog.context.bot->sendMessage(message.chat_id, "Enter day number", def::KeyboardType::none);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingDayNumber(stateMachine, message);
    };
}

void EditTimeState::onWaitingDayNumber(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingDayNumber", _name);

    if (const auto res = def::toInt(message.text); !res) {
        SPDLOG_WARN("Wrong value - {}", message.text);
        dialog.context.bot->sendMessage(message.chat_id, "Wrong value",
                                        def::KeyboardType::keyboardChooseCommands);
        dialog.setState<IdleState>();
        return;
    } else {
        SPDLOG_INFO("Add day: {}", message.text);
        dialog.context.dayNumber = *res;
    }

    dialog.context.bot->sendMessage(message.chat_id, "choose mount of minutes",
                                    def::KeyboardType::none);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingMinutes(stateMachine, message);
    };
}

void EditTimeState::onWaitingMinutes(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingMinutes", _name);
    auto& dialogContext = dialog.context;

    if (const auto res = def::toInt(message.text); !res) {
        SPDLOG_WARN("Wrong value - {}", message.text);
        dialog.context.bot->sendMessage(message.chat_id, "Wrong value",
                                        def::KeyboardType::keyboardChooseCommands);
        dialog.setState<IdleState>();
        return;
    } else {
        SPDLOG_INFO("Add time {} to date {}.{}", message.text, dialogContext.monthNumber,
                    dialogContext.dayNumber);
        dialogContext.data.stat->years[dialog.context.yearNumber][dialogContext.monthNumber].edit(
            dialogContext.dayNumber, std::chrono::minutes{*res});
    }

    std::string res{"Value is added, current state: "};
    res += std::to_string(dialogContext.dayNumber) + " : " + message.text;
    dialogContext.bot->sendMessage(message.chat_id, res, def::KeyboardType::keyboardChooseCommands);
    dialog.setState<IdleState>();
}

}; // namespace fsm
