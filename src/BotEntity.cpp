#include "BotEntity.h"

#include <string>

#include "Command.h"
#include "CommandBuilder.h"
#include "def.h"
#include "logger/Logger.h"
#include "tgbot/EventBroadcaster.h"

namespace core {

bool BotEntity::initBot() {
    SPDLOG_INFO("Bot username: {}\n", m_bot.getApi().getMe()->username.c_str());

    m_bot.getApi().deleteWebhook();
    m_bot.getApi().deleteMyCommands();

    TgBot::ReplyKeyboardMarkup::Ptr keyboardWithLayout(new TgBot::ReplyKeyboardMarkup);
    def::createKeyboard(def::keyboardWithLayoutVector, keyboardWithLayout);

    TgBot::ReplyKeyboardMarkup::Ptr keyboardChooseMonth(new TgBot::ReplyKeyboardMarkup);
    def::createKeyboard(def::keyboardChooseMonthVector, keyboardChooseMonth);

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

    m_bot.getEvents().onNonCommandMessage([&](TgBot::Message::Ptr message) { m_handler.execute(message); });

    SPDLOG_INFO("Bot init succeess");
    return true;
}

TgBot::TgLongPoll BotEntity::longPollObj() { return TgBot::TgLongPoll(m_bot); }

};  // namespace core
