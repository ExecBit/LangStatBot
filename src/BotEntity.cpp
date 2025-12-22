#include "BotEntity.h"

#include "CommandBuilder.h"
#include "tgbot/EventBroadcaster.h"

namespace {

void createOneColumnKeyboard(const std::vector<std::string>& buttonStrings, TgBot::ReplyKeyboardMarkup::Ptr& kb) {
    for (size_t i = 0; i < buttonStrings.size(); ++i) {
        std::vector<TgBot::KeyboardButton::Ptr> row;
        TgBot::KeyboardButton::Ptr button(new TgBot::KeyboardButton);
        button->text = buttonStrings[i];
        button->requestContact = false;
        button->requestLocation = false;
        row.push_back(button);
        kb->keyboard.push_back(row);
    }
}

}  // namespace

namespace core {

bool BotEntity::initBot() {
    printf("Bot username: %s\n", m_bot.getApi().getMe()->username.c_str());

    m_bot.getApi().deleteWebhook();
    m_bot.getApi().deleteMyCommands();

    TgBot::ReplyKeyboardMarkup::Ptr keyboardOneCol(new TgBot::ReplyKeyboardMarkup);
    createOneColumnKeyboard({"show stat current month", "add time", "Option 3"}, keyboardOneCol);

    CommandBuilder builder{m_bot};

    builder
        .setCommand("start",
                    [&bot = m_bot](TgBot::Message::Ptr message) { bot.getApi().sendMessage(message->chat->id, "Hi!"); })
        .setCommand("add_word",
                    [&bot = m_bot, &test_text_state = m_test_text_state](TgBot::Message::Ptr message) {
                        printf("Enter text log\n");
                        bot.getApi().sendMessage(message->chat->id, "Enter text");
                        test_text_state = true;
                    })

        .setCommand("show_words",
                    [&bot = m_bot, &data = m_data](TgBot::Message::Ptr message) {
                        if (data.words.empty()) {
                            bot.getApi().sendMessage(message->chat->id, "Dictionary is empty");
                            return;
                        }

                        std::string msg;
                        for (const auto& word : data.words) {
                            msg += word + '\n';
                        }
                        bot.getApi().sendMessage(message->chat->id, msg);
                    })

        .setCommand("stat",
                    [&bot = m_bot, keyboardOneCol](TgBot::Message::Ptr message) {
                        bot.getApi().sendMessage(message->chat->id,
                                                 "choose option",
                                                 nullptr, nullptr, keyboardOneCol);
                    })

        .build();


    m_bot.getEvents().onNonCommandMessage([&bot = m_bot, &data = m_data, &test_text_state = m_test_text_state,
                                    &bot_commands = m_bot_commands](TgBot::Message::Ptr message) {
        printf("Get message: %s\n", message->text.c_str());

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

    return true;
}

TgBot::TgLongPoll BotEntity::longPollObj() { return TgBot::TgLongPoll(m_bot); }

};  // namespace core
