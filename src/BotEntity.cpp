#include "BotEntity.h"

namespace core {

bool BotEntity::initBot() {
    printf("Bot username: %s\n", m_bot.getApi().getMe()->username.c_str());

    m_bot.getApi().deleteWebhook();
    m_bot.getApi().deleteMyCommands();

    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "add_word";
    cmdArray->description = "add word to dictionary";
    commands.push_back(cmdArray);

    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "show_words";
    cmdArray->description = "show current dictionary";
    commands.push_back(cmdArray);

    m_bot.getApi().setMyCommands(commands);

    m_bot.getEvents().onCommand(
        "start", [&bot = m_bot](TgBot::Message::Ptr message) { bot.getApi().sendMessage(message->chat->id, "Hi!"); });

    m_bot.getEvents().onCommand("add_word",
                                [&bot = m_bot, &test_text_state = m_test_text_state](TgBot::Message::Ptr message) {
                                    printf("Enter text log");
                                    bot.getApi().sendMessage(message->chat->id, "Enter text");
                                    test_text_state = true;
                                });

    m_bot.getEvents().onAnyMessage([&bot = m_bot, &data = m_data, &test_text_state = m_test_text_state,
                                    &bot_commands = m_bot_commands](TgBot::Message::Ptr message) {
        if (test_text_state) {
            printf("Add word: %s\n", message->text.c_str());
            data.words.push_back(message->text);
            bot.getApi().sendMessage(message->chat->id, "ok");
            test_text_state = false;
            return;
        }

        for (const auto& command : bot_commands) {
            if ("/" + command == message->text) {
                return;
            }
        }

        bot.getApi().sendMessage(message->chat->id, "unknown command");
    });

    m_bot.getEvents().onCommand("show_words", [&bot = m_bot, &data = m_data](TgBot::Message::Ptr message) {
        if (data.words.empty()) {
            bot.getApi().sendMessage(message->chat->id, "Dictionary is empty");
            return;
        }

        std::string msg;
        for (const auto& word : data.words) {
            msg += word + '\n';
        }
        bot.getApi().sendMessage(message->chat->id, msg);
    });

    return true;
}

TgBot::TgLongPoll BotEntity::longPollObj() { return TgBot::TgLongPoll(m_bot); }

};  // namespace core
