#include "EditTimeState.h"
#include "IdleState.h"
#include "logger/Logger.h"
#include "fsm/StateMachine.h"

namespace fsm {
EditTimeState::EditTimeState(const core::Message& message) : initMessage(message) {}

void EditTimeState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    dialog.context.bot->sendMessage(initMessage.chat_id, "Enter month number",
                                    def::KeyboardType::keyboardChooseMonth);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingMonthNumber(stateMachine, message);
    };
}

void EditTimeState::onWaitingMonthNumber(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingMonthNumber", _name);

    SPDLOG_INFO("Add month number: {}", message.text);
    dialog.context.bot->sendMessage(message.chat_id, "choose day", def::KeyboardType::none);

    dialog.context.monthNumber = stoi(message.text);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingDayNumber(stateMachine, message);
    };
}

void EditTimeState::onWaitingDayNumber(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingDayNumber", _name);
    SPDLOG_INFO("Add day: {}", message.text);

    dialog.context.bot->sendMessage(message.chat_id, "choose mount of minutes",
                                    def::KeyboardType::none);
    dialog.context.dayNumber = stoi(message.text);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingMinutes(stateMachine, message);
    };
}

void EditTimeState::onWaitingMinutes(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingMinutes", _name);
    auto& dialogContext = dialog.context;
    SPDLOG_INFO("Add time {} to date {}.{}", message.text, dialogContext.monthNumber,
                dialogContext.dayNumber);

    dialogContext.data.stat->years.at(2025)
        .at(dialogContext.monthNumber)
        .edit(dialogContext.dayNumber, std::chrono::minutes{stoi(message.text)});

    //      dialogContext.data.stat->years.at(2025).at(dialogContext.monthNumber).days[dialogContext.dayNumber]
    //      =
    //          std::chrono::minutes{stoi(message->text)};

    std::string res{"Value is added, current state: "};
    res += std::to_string(dialogContext.dayNumber) + " : " + message.text;
    dialogContext.bot->sendMessage(message.chat_id, res, def::KeyboardType::keyboardWithLayout);
    dialog.setState<IdleState>();
}
}; // namespace fsm
