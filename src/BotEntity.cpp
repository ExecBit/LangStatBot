#include "BotEntity.h"

namespace core {

bool BotEntity::initBot() {
    SPDLOG_INFO("Bot username: {}", m_bot->username());

    m_bot->deleteWebhook();
    m_bot->deleteMyCommands();

    m_bot->setCommands();
    m_bot->setCallback(m_handler);

    SPDLOG_INFO("Bot init succeess");
    return true;
}

}; // namespace core
