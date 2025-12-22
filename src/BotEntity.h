#pragma once

#include <tgbot/tgbot.h>

#include <memory>
#include <string_view>
#include "Data.h"
#include "tgbot/EventBroadcaster.h"

namespace core {

class BotEntity {
public:
    explicit BotEntity(std::string_view token, Data& data) : m_bot{std::string{token}}, m_data{data} {}

    bool initBot();
    TgBot::TgLongPoll longPollObj();

private:
    using MessageCallback = std::function<void (const TgBot::Message::Ptr)>;
    TgBot::Bot m_bot;
    Data& m_data;

    bool m_test_text_state{false};

    std::vector<std::string> m_bot_commands = {"start", "add_word", "show_words", "stat", "layout"};
};

};  // namespace core
