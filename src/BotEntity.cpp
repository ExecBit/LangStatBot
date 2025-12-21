#include "BotEntity.h"

namespace core {

bool BotEntity::initBot() {
    printf("Bot username: %s\n", m_bot.getApi().getMe()->username.c_str());

    m_bot.getApi().deleteWebhook();
    m_bot.getApi().deleteMyCommands();

    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "add_time";
    cmdArray->description = "add time to current day";
    commands.push_back(cmdArray);

    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "show";
    cmdArray->description = "show current time";
    commands.push_back(cmdArray);

    m_bot.getApi().setMyCommands(commands);

    m_bot.getEvents().onCommand(
        "start", [&bot = m_bot](TgBot::Message::Ptr message) { bot.getApi().sendMessage(message->chat->id, "Hi!"); });

    std::string result{"fuck"};
    m_bot.getEvents().onCommand("show", [&bot = m_bot, result](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, result);
    });

    m_bot.getEvents().onAnyMessage([&bot = m_bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    return true;
}

TgBot::TgLongPoll BotEntity::longPollObj() {
    return TgBot::TgLongPoll(m_bot);
}

};  // namespace core
