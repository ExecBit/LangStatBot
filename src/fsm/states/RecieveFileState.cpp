#include "RecieveFileState.h"
#include "IdleState.h"

#include "fsm/StateMachine.h"
#include "common/def.h"
#include "logger/Logger.h"

#include <memory>
#include <string>

namespace fsm {
RecieveFileState::RecieveFileState(const core::Message& message) : initMessage(message) {}

void RecieveFileState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    auto& dialogContext = dialog.context;

    auto input_file(std::make_shared<TgBot::InputFile>());
    input_file->data = "error";
    input_file->mimeType = "application/json";
    input_file->fileName = "data.json";

    if (const auto& rawString = dialogContext.dataMgr->save(dialogContext.data, "./data.json");
        !rawString.empty()) {
        input_file->data = rawString;
    }

    dialogContext.bot->sendDocument(
        initMessage.chat_id,
        {.data = "error", .mimeType = "application/json", .fileName = "data.json"},
        def::KeyboardType::none);

    dialog.setState<IdleState>();
}
}; // namespace fsm
