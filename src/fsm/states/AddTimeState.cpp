#include "AddTimeState.h"
#include "IdleState.h"

#include <chrono>
#include "logger/Logger.h"
#include "fsm/StateMachine.h"

namespace fsm {
AddTimeState::AddTimeState(const core::Message& message) : initMessage(message) {}

void AddTimeState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    dialog.context.bot->sendMessage(initMessage.chat_id, "choose mount of minutes",
                                    def::KeyboardType::none);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
        return onWaitingMinutes(stateMachine, message);
    };
}

void AddTimeState::onWaitingMinutes(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onWaitingMinutes", _name);

    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::floor<std::chrono::days>(now);
    auto ymd = std::chrono::year_month_day(today);

    int year = static_cast<int>(ymd.year());
    unsigned month = static_cast<unsigned>(ymd.month());
    unsigned day = static_cast<unsigned>(ymd.day());

    auto& dialogContext = dialog.context;
    SPDLOG_INFO("Add time {} to date {}.{}", message.text, month, day);

    // auto& minutesCount =
    // dialogContext.data.stat->years.at(year).at(month).readDays().at(day);
    auto minutesCount = std::chrono::minutes{};
    const auto& map = dialogContext.data.stat->years.at(year).at(month).readDays();
    if (const auto itDay = map.find(day); itDay != map.end()) {
        minutesCount = itDay->second;
    }

    // = dialogContext.data.stat->years.at(year).at(month).readDays().find(day);
    const auto sourceTime = std::to_string(minutesCount.count());

    const auto currMinutes = dialogContext.data.stat->years.at(year).at(month).add(
        day, std::chrono::minutes{stoi(message.text)});

    // minutesCount += std::chrono::minutes{stoi(message->text)};
    const auto res = "Value is added, current state - " + std::to_string(day) + " : " +
                     std::to_string(currMinutes.count()) + " (" + sourceTime + " + " +
                     message.text + ")";

    dialogContext.bot->sendMessage(message.chat_id, res, def::KeyboardType::keyboardWithLayout);
    dialog.setState<IdleState>();
}
}; // namespace fsm
