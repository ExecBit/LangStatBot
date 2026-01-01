#include "BotEntity.h"

namespace core {

bool BotEntity::initBot() {
    SPDLOG_INFO("Bot username: {}", m_bot->username());

    SPDLOG_INFO("deleteWebhook");
    m_bot->deleteWebhook();
    SPDLOG_INFO("deleteMyCommands");
    m_bot->deleteMyCommands();

    SPDLOG_INFO("setCommands");
    m_bot->setCommands();
    SPDLOG_INFO("setCallback");
    m_bot->setCallback(m_handler);

    SPDLOG_INFO("Bot init succeess");
    return true;
}

}; // namespace core
