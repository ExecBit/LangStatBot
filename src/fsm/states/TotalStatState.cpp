#include "TotalStatState.h"

#include "IdleState.h"
#include "common/def.h"
#include "fsm/StateMachine.h"
#include "logger/Logger.h"

namespace fsm {
TotalStatState::TotalStatState(const core::Message& message) : initMessage(message) {}

void TotalStatState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    auto& dialogContext = dialog.context;


    std::chrono::minutes total{0};

    for (const auto& [year, ptr] : dialogContext.data.stat->years) {
        for (const auto& [monthNum, ptr] : dialogContext.data.stat->years[year]) {
            for (const auto& day : dialogContext.data.stat->years[year][monthNum].days()) {
                total += day.second;
            }
        }

    }

    const auto hours = std::chrono::duration_cast<std::chrono::hours>(total);
    const auto minutes = total - hours;
    std::stringstream ss;

    ss << "Total Statitic\n"
        << "total time: " << hours << " " << minutes << '\n';

    dialog.context.bot->sendMessage(initMessage.chat_id, ss.str(),
                                    def::KeyboardType::keyboardChooseCommands);

    dialog.setState<IdleState>();
}

}; // namespace fsm
