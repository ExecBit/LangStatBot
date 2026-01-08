#include "RecieveFileState.h"

#include "IdleState.h"
#include "common/def.h"
#include "fsm/StateMachine.h"
#include "logger/Logger.h"

#include <memory>
#include <string>

namespace fsm {
RecieveFileState::RecieveFileState(const core::Message& message) : initMessage(message) {}

void RecieveFileState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    auto& dialogContext = dialog.context;

    std::string data{"error"};

    if (const auto& rawString = dialogContext.dataMgr->save(dialogContext.data, "./data.json");
        !rawString.empty()) {
        data = rawString;
    }

    dialogContext.bot->sendDocument(
        initMessage.chat_id,
        {.data = data, .mimeType = "application/json", .fileName = "data.json"},
        def::KeyboardType::none);

    dialog.setState<IdleState>();
}
}; // namespace fsm
