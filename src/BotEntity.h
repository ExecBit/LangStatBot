#pragma once

#include <tgbot/tgbot.h>

#include <memory>

namespace core {

class BotEntity {
public:
    explicit BotEntity(std::string_view token) : m_bot{std::string{token}} {}

    bool initBot();
    TgBot::TgLongPoll longPollObj();

private:
    TgBot::Bot m_bot;
};

};  // namespace core
