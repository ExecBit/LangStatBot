#include "BotEntity.h"

#include "CommandBuilder.h"
#include "tgbot/EventBroadcaster.h"

namespace {

using namespace std;
using namespace TgBot;

void createKeyboard(const vector<vector<string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb) {
    for (size_t i = 0; i < buttonLayout.size(); ++i) {
        vector<KeyboardButton::Ptr> row;
        for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
            KeyboardButton::Ptr button(new KeyboardButton);
            button->text = "/" + buttonLayout[i][j];
            button->requestContact = false;
            button->requestLocation = false;
            row.push_back(button);
        }
        kb->keyboard.push_back(row);
    }
}

}  // namespace

namespace core {

bool BotEntity::initBot() {
    printf("Bot username: %s\n", m_bot.getApi().getMe()->username.c_str());

    m_bot.getApi().deleteWebhook();
    m_bot.getApi().deleteMyCommands();

    ReplyKeyboardMarkup::Ptr keyboardWithLayout(new ReplyKeyboardMarkup);
    createKeyboard({{"ping", "add_word"}, {"show_words", "add_time", "show_time"}}, keyboardWithLayout);

    CommandBuilder builder{m_bot};

    builder
        .setCommand("ping",
                    [&bot = m_bot, keyboardWithLayout](TgBot::Message::Ptr message) {
                        bot.getApi().sendMessage(message->chat->id, "pong", nullptr, nullptr, keyboardWithLayout);
                    })
        .setCommand("start",
                    [&bot = m_bot, keyboardWithLayout](TgBot::Message::Ptr message) {
                        bot.getApi().sendMessage(message->chat->id, "choose option", nullptr, nullptr,
                                                 keyboardWithLayout);
                    })
        .setCommand("add_word",
                    [&bot = m_bot,
                     &isWaitingWordToDictionary = m_context.isWaitingWordToDictionary](TgBot::Message::Ptr message) {
                        printf("Enter text log\n");
                        bot.getApi().sendMessage(message->chat->id, "Enter text");
                        isWaitingWordToDictionary = true;
                    })

        .setCommand("show_words",
                    [&bot = m_bot, &data = m_data, keyboardWithLayout](TgBot::Message::Ptr message) {
                        if (data.words.empty()) {
                            bot.getApi().sendMessage(message->chat->id, "Dictionary is empty", nullptr, nullptr,
                                                     keyboardWithLayout);
                            return;
                        }

                        std::string msg;
                        for (const auto& word : data.words) {
                            msg += word + '\n';
                        }
                        bot.getApi().sendMessage(message->chat->id, msg, nullptr, nullptr, keyboardWithLayout);
                    })

        .setCommand("add_time",
                    [&bot = m_bot, &isWaitingTime = m_context.isWaitingTime](TgBot::Message::Ptr message) {
                        printf("Enter text log\n");
                        bot.getApi().sendMessage(message->chat->id, "Enter text");
                        isWaitingTime = true;
                    })
        .setCommand("show_time",
                    [&bot = m_bot, &data = m_data, keyboardWithLayout](TgBot::Message::Ptr message) {
                        if (!data.stat) {
                            bot.getApi().sendMessage(message->chat->id, "Sheduler is empty", nullptr, nullptr,
                                                     keyboardWithLayout);
                            return;
                        }

                        std::string msg{"in working"};
                        bot.getApi().sendMessage(message->chat->id, msg);
                    })
        .build();

    m_bot.getEvents().onNonCommandMessage(
        [&bot = m_bot, &data = m_data, &context = m_context, keyboardWithLayout](TgBot::Message::Ptr message) {
            printf("Get message: %s\n", message->text.c_str());

            if (context.isWaitingWordToDictionary) {
                printf("Add word: %s\n", message->text.c_str());
                data.words.push_back(message->text);
                bot.getApi().sendMessage(message->chat->id, "ok", nullptr, nullptr, keyboardWithLayout);
                context.isWaitingWordToDictionary = false;
                return;
            }

            if (context.isWaitingTime) {
                printf("Add time: %s\n", message->text.c_str());
                // data.words.push_back(message->text);
                bot.getApi().sendMessage(message->chat->id, "ok", nullptr, nullptr, keyboardWithLayout);
                context.isWaitingTime = false;
                return;
            }

            for (const auto& command : context.m_bot_commands) {
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
