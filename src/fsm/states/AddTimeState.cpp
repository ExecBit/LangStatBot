#include "AddTimeState.h"

#include "IdleState.h"
#include "fsm/StateMachine.h"
#include "logger/Logger.h"

#include <chrono>

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

    const auto year = static_cast<int32_t>(ymd.year());
    const auto month = static_cast<uint32_t>(ymd.month());
    const auto day = static_cast<uint32_t>(ymd.day());

    auto& dialogContext = dialog.context;
    SPDLOG_INFO("Add time {} to date {}.{}", message.text, month, day);

    auto minutesCount = std::chrono::minutes{};
    const auto& map = dialogContext.data.stat->years[year][month].readDays();
    if (const auto itDay = map.find(day); itDay != map.end()) {
        minutesCount = itDay->second;
    }
    const auto sourceTime = std::to_string(minutesCount.count());

    auto currMinutes = std::chrono::minutes{};
    if (const auto res = def::toInt(message.text); !res) {
        SPDLOG_WARN("Wrong value - {}", message.text);
        dialogContext.bot->sendMessage(message.chat_id, "Wrong value",
                                       def::KeyboardType::keyboardChooseCommands);
        dialog.setState<IdleState>();
        return;
    } else {
        dialogContext.data.stat->years[year][month].add(day, std::chrono::minutes{*res});
    }

    const auto res = "Value is added, current state - " + std::to_string(day) + " : " +
                     std::to_string(currMinutes.count()) + " (" + sourceTime + " + " +
                     message.text + ")";

    dialogContext.bot->sendMessage(message.chat_id, res, def::KeyboardType::keyboardChooseCommands);
    dialog.setState<IdleState>();
}
}; // namespace fsm
