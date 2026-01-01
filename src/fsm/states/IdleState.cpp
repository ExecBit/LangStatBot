#include "IdleState.h"
#include "AddWordState.h"
#include "ShowWordState.h"
#include "ShowTimeState.h"
#include "AddTimeState.h"
#include "EditTimeState.h"
#include "DumpDataState.h"
#include "EditTimeState.h"
#include "RecieveFileState.h"

#include "logger/Logger.h"
#include "fsm/StateMachine.h"
#include "common/Command.h"

namespace fsm {
void IdleState::onEnter(StateMachine& dialog) {
    SPDLOG_INFO("{} - onEnter", _name);

    dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& msg) {
        return onMessage(stateMachine, msg);
    };
}

void IdleState::onMessage(StateMachine& dialog, const core::Message& message) {
    SPDLOG_INFO("{} - onMessage", _name);

    switch (command::parse(message.text)) {
    case command::Type::addWord: {
        dialog.setState<AddWordState>(message);
    }; break;
    case command::Type::showWord: {
        dialog.setState<ShowWordState>(message);
    }; break;
    case command::Type::addTime: {
        dialog.setState<AddTimeState>(message);
    }; break;
    case command::Type::showTime: {
        dialog.setState<ShowTimeState>(message);
    } break;
    case command::Type::dumpData: {
        dialog.setState<DumpDataState>(message);
    } break;
    case command::Type::recieveFile: {
        dialog.setState<RecieveFileState>(message);
    } break;
    case command::Type::editTime: {
        dialog.setState<EditTimeState>(message);
    } break;
    case command::Type::unknown: {
        dialog.context.bot->sendMessage(message.chat_id, "unknown command",
                                        def::KeyboardType::keyboardChooseCommands);

        dialog.setState<IdleState>();
    }; break;
    }
    return;
}
}; // namespace fsm
