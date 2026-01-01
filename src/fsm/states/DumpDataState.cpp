#include "DumpDataState.h"
#include "IdleState.h"
#include "logger/Logger.h"
#include "fsm/StateMachine.h"

namespace fsm {
DumpDataState::DumpDataState(const core::Message& message) : initMessage(message) {}

void DumpDataState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    auto& dialogContext = dialog.context;

    if (const auto& rawString = dialogContext.dataMgr->save(dialogContext.data, "./data.json");
        rawString.empty()) {
        dialogContext.bot->sendMessage(initMessage.chat_id, "error",
                                       def::KeyboardType::keyboardChooseCommands);
    } else {
        dialogContext.bot->sendMessage(initMessage.chat_id, "success",
                                       def::KeyboardType::keyboardChooseCommands);
    }

    dialog.setState<IdleState>();
}
}; // namespace fsm
