#include "CommandBuilder.h"


namespace core {

CommandBuilder& CommandBuilder::setCommand(std::string_view name, MessageCallback callback) {
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = name;
    cmdArray->description = "...";
    commands.push_back(cmdArray);
    m_bot.getEvents().onCommand(name.data(), callback);

    return *this;
}

};  // namespace core
