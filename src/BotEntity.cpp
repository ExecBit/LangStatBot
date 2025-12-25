#include "BotEntity.h"

#include <string>

#include "Command.h"
#include "CommandBuilder.h"
#include "logger/Logger.h"
#include "tgbot/EventBroadcaster.h"

//  namespace {

//  using namespace std;
//  using namespace TgBot;

//  void createKeyboard(const vector<vector<string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb) {
//      for (size_t i = 0; i < buttonLayout.size(); ++i) {
//          vector<KeyboardButton::Ptr> row;
//          for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
//              KeyboardButton::Ptr button(new KeyboardButton);
//              button->text = buttonLayout[i][j];
//              button->requestContact = false;
//              button->requestLocation = false;
//              row.push_back(button);
//          }
//          kb->keyboard.push_back(row);
//      }
//  }

//  }  // namespace

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
        [&](TgBot::Message::Ptr message) {
            m_handler.execute(message);
        });

    SPDLOG_INFO("Bot init succeess");
    return true;
}

TgBot::TgLongPoll BotEntity::longPollObj() { return TgBot::TgLongPoll(m_bot); }

};  // namespace core
