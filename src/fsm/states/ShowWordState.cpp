#include "ShowWordState.h"
#include "IdleState.h"

#include "core/Data.h"
#include "fsm/StateMachine.h"
#include "common/def.h"
#include "logger/Logger.h"

#include <memory>
#include <string>


namespace fsm {
ShowWordState::ShowWordState(const core::Message& message) : initMessage(message) {}

void ShowWordState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    if (dialog.context.data.stat->words.empty()) {
        dialog.context.bot->sendMessage(initMessage.chat_id, "Dictionary is empty",
                                        def::KeyboardType::keyboardChooseCommands);
        return;
    }

    std::string msg;
    for (const auto& word : dialog.context.data.stat->words) {
        msg += word + '\n';
    }
    dialog.context.bot->sendMessage(initMessage.chat_id, msg,
                                    def::KeyboardType::keyboardChooseCommands);

    dialog.setState<IdleState>();
}
}; // namespace fsm
