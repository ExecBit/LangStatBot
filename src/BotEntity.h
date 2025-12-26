#pragma once

#include <tgbot/tgbot.h>

#include <memory>
#include <string_view>
#include <unordered_map>

#include "Data.h"
#include "DataManager.h"
#include "fsm/StateMachine.h"
#include "tgbot/EventBroadcaster.h"

namespace core {

class BotEntity {
public:
    explicit BotEntity(std::string_view token, Data& data, io_interface::IDataManager* dataMgr)
        : m_bot{std::string{token}}
        , m_data{data}
        , m_handler(m_bot, m_data, dataMgr) {}

    bool initBot();
    TgBot::TgLongPoll longPollObj();

private:
    using MessageCallback = std::function<void(const TgBot::Message::Ptr)>;
    TgBot::Bot m_bot;
    Data& m_data;
    fsm::StateMachine m_handler;
};

};  // namespace core
