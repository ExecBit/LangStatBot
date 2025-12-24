#include "BotEntity.h"

#include <string>

#include "CommandBuilder.h"
#include "logger/Logger.h"
#include "tgbot/EventBroadcaster.h"

namespace {

using namespace std;
using namespace TgBot;

void createKeyboard(const vector<vector<string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb) {
    for (size_t i = 0; i < buttonLayout.size(); ++i) {
        vector<KeyboardButton::Ptr> row;
        for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
            KeyboardButton::Ptr button(new KeyboardButton);
            button->text = buttonLayout[i][j];
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
    SPDLOG_INFO("Bot username: {}\n", m_bot.getApi().getMe()->username.c_str());

    m_bot.getApi().deleteWebhook();
    m_bot.getApi().deleteMyCommands();

    ReplyKeyboardMarkup::Ptr keyboardWithLayout(new ReplyKeyboardMarkup);
    createKeyboard({{"add word", "add time"}, {"show words", "show time"}}, keyboardWithLayout);

    ReplyKeyboardMarkup::Ptr keyboardChooseMonth(new ReplyKeyboardMarkup);
    createKeyboard(
        {
            {"1", "2", "3", "4", "5", "6"},
            {"7", "8", "9", "10", "11", "12"},
        },
        keyboardChooseMonth);

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
        .build();

    m_bot.getEvents().onNonCommandMessage(
        //[&bot = m_bot, &data = m_data, &context = m_context, keyboardWithLayout](TgBot::Message::Ptr message) {
        [&, &bot = m_bot, &data = m_data, &context = m_context, keyboardWithLayout,
         keyboardChooseMonth](TgBot::Message::Ptr message) {
            SPDLOG_INFO("Get message: {}", message->text);

            if (context.isWaitingWordToDictionary) {
                SPDLOG_INFO("Add word: {}", message->text);
                data.stat->words.push_back(message->text);
                bot.getApi().sendMessage(message->chat->id, "ok", nullptr, nullptr, keyboardWithLayout);
                context.isWaitingWordToDictionary = false;
                return;
            }

            if (context.isWaitingMonthNumber) {
                SPDLOG_INFO("Add month number: {}", message->text);
                // data.words.push_back(message->text);
                bot.getApi().sendMessage(message->chat->id, "choose day", nullptr, nullptr, keyboardChooseMonth);

                context.monthNumber = stoi(message->text);
                context.isWaitingMonthNumber = false;
                context.isWaitingDayNumber = true;
                return;
            }

            if (context.isWaitingDayNumber) {
                SPDLOG_INFO("Add day: {}", message->text);

                bot.getApi().sendMessage(message->chat->id, "choose mount of minutes");

                context.dayNumber = stoi(message->text);
                context.isWaitingDayNumber = false;
                context.isWaitingMinutes = true;
                return;
            }

            if (context.isWaitingMinutes) {
                SPDLOG_INFO("Add time {} to date {}.{}", message->text, context.monthNumber, context.dayNumber);

                m_data.stat->years.at(2025).at(context.monthNumber).days[context.dayNumber] = stoi(message->text);

                bot.getApi().sendMessage(message->chat->id, "ok", nullptr, nullptr, keyboardWithLayout);
                //              if (const auto res = m_data.stat->years.at(2025)
                //                                       .at(context.monthNumber)
                //                                       .days.insert_or_assign(context.dayNumber, stoi(message->text));
                //                  res.second) {
                //                  bot.getApi().sendMessage(message->chat->id, "ok", nullptr, nullptr,
                //                  keyboardWithLayout);
                //              } else {
                //                  bot.getApi().sendMessage(message->chat->id, "Error, value is not assign", nullptr,
                //                  nullptr,
                //                                           keyboardWithLayout);
                //              }
                context.isWaitingMinutes = false;
                return;
            }

            if (context.isWaitingShowtime) {
                SPDLOG_INFO("Got Month number {}", message->text);

                const auto& monthStat = m_data.stat->years.at(2025).at(stoi(message->text));

                bot.getApi().sendMessage(message->chat->id, monthStat.print(), nullptr, nullptr, keyboardWithLayout);
                context.isWaitingShowtime = false;
                return;
            }

            if (const auto command = parseCommand(message->text); command) {
                switch (command.value()) {
                    case Command::addWord: {
                        bot.getApi().sendMessage(message->chat->id, "Enter text");
                        context.isWaitingWordToDictionary = true;
                    }; break;
                    case Command::showWord: {
                        if (data.stat->words.empty()) {
                            bot.getApi().sendMessage(message->chat->id, "Dictionary is empty", nullptr, nullptr,
                                                     keyboardWithLayout);
                            return;
                        }

                        std::string msg;
                        for (const auto& word : data.stat->words) {
                            msg += word + '\n';
                        }
                        bot.getApi().sendMessage(message->chat->id, msg, nullptr, nullptr, keyboardWithLayout);
                    }; break;
                    case Command::addTime: {
                        bot.getApi().sendMessage(message->chat->id, "Enter month number");
                        context.isWaitingMonthNumber = true;
                    }; break;
                    case Command::showTime: {
                        if (!data.stat) {
                            bot.getApi().sendMessage(message->chat->id, "Sheduler is empty", nullptr, nullptr,
                                                     keyboardWithLayout);
                            return;
                        }

                        bot.getApi().sendMessage(message->chat->id, "Enter month number", nullptr, nullptr, keyboardChooseMonth);
                        context.isWaitingShowtime = true;
                    } break;
                    case Command::unknown: {
                    }; break;
                }
                return;
            }

            for (const auto& command : context.m_bot_commands) {
                if ("/" + command == message->text) {
                    return;
                }
            }

            bot.getApi().sendMessage(message->chat->id, "unknown command", nullptr, nullptr, keyboardWithLayout);
        });

    SPDLOG_INFO("Bot init succeess");
    return true;
}

TgBot::TgLongPoll BotEntity::longPollObj() { return TgBot::TgLongPoll(m_bot); }

};  // namespace core
