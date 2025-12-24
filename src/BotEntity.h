#pragma once

#include <tgbot/tgbot.h>

#include <memory>
#include <string_view>
#include <unordered_map>

#include "Data.h"
#include "tgbot/EventBroadcaster.h"

namespace core {

struct BotContext {
    bool isWaitingWordToDictionary{false};
    bool isWaitingStatOption{false};
    bool isWaitingMonthNumber{false};
    bool isWaitingDayNumber{false};
    bool isWaitingMinutes{false};
    bool isWaitingShowtime{false};

    int monthNumber{};
    int dayNumber{};

    std::vector<std::string> m_bot_commands = {"start", "add_word", "show_words", "stat", "layout"};
};

class BotEntity {
public:
    explicit BotEntity(std::string_view token, Data& data) : m_bot{std::string{token}}, m_data{data} {}

    bool initBot();
    TgBot::TgLongPoll longPollObj();

private:
    using MessageCallback = std::function<void(const TgBot::Message::Ptr)>;
    TgBot::Bot m_bot;
    Data& m_data;

    BotContext m_context;

    enum class Command : uint32_t { addWord, showWord, addTime, showTime, unknown };

    struct pair {
        std::string_view key;
        Command value;
    };

    static constexpr std::array commands = {pair{"add word", Command::addWord}, pair{"show words", Command::showWord},
                                            pair{"add time", Command::addTime}, pair{"show time", Command::showTime}};

    constexpr std::optional<Command> parseCommand(std::string_view s) {
        for (const auto& e : commands) {
            if (e.key == s) return e.value;
        }
        return std::nullopt;
    }
};

};  // namespace core
