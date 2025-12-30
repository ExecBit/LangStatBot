#pragma once

#include "Data.h"
#include "DataManager.h"
#include "IBot.h"
#include "TelegramBotAdapter.h"
#include "fsm/StateMachine.h"
#include "tgbot/EventBroadcaster.h"

#include <memory>
#include <string_view>
#include <tgbot/tgbot.h>
#include <unordered_map>

namespace core {

class BotEntity {
  public:
    explicit BotEntity(std::string_view token, Data& data, io_interface::IDataManager* dataMgr)
        : m_bot{std::make_unique<TelegramBotAdapter>(std::string{token})}, m_data{data},
          m_handler(m_bot.get(), m_data, dataMgr) {}

    bool initBot();
    void startPoll() { m_bot->startLongPoll(); }

  private:
    std::unique_ptr<IBot> m_bot;
    // TgBot::Bot m_bot;
    Data& m_data;
    fsm::StateMachine m_handler;
};

}; // namespace core
