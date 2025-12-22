#pragma once

#include <tgbot/tgbot.h>

namespace core {

class CommandBuilder {
    using MessageCallback = std::function<void(const TgBot::Message::Ptr)>;

public:
    explicit CommandBuilder(TgBot::Bot& bot) : m_bot(bot) {}

    CommandBuilder& setCommand(std::string_view name, MessageCallback callback);
    void build() { m_bot.getApi().setMyCommands(commands); }

private:
    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::Bot& m_bot;
};

};  // namespace core
