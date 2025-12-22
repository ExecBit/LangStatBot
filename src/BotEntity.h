#pragma once

#include <tgbot/tgbot.h>

#include <memory>
#include "Data.h"

namespace core {

class BotEntity {
public:
    explicit BotEntity(std::string_view token, Data& data) : m_bot{std::string{token}}, m_data{data} {}

    bool initBot();
    TgBot::TgLongPoll longPollObj();

private:
    TgBot::Bot m_bot;
    Data& m_data;

    std::atomic_bool m_test_text_state{false};

    std::vector<std::string> m_bot_commands = {"start", "add_word", "show_words"};
};

};  // namespace core
