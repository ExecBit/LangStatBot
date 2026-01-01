#pragma once

#include "CommandBuilder.h"
#include "core/IBot.h"
#include "Message.h"
#include "common/def.h"
#include "fsm/StateMachine.h"

#include <memory>

namespace core {

class TelegramBotAdapter : public IBot {
    TgBot::Bot m_bot;
    TgBot::ReplyKeyboardMarkup::Ptr keyboardChooseCommands;
    TgBot::ReplyKeyboardMarkup::Ptr keyboardChooseMonth;
    TgBot::ReplyKeyboardMarkup::Ptr keyboardChooseYear;

  public:
    explicit TelegramBotAdapter(const std::string& token);
    void sendMessage(std::int64_t chatId, const std::string& text,
                     def::KeyboardType keyboardType) override;
    void sendDocument(std::int64_t chatId, const def::DocumentData& doc,
                      def::KeyboardType) override;

    void deleteWebhook() override;
    void deleteMyCommands() override;
    std::string username() override;
    void setCommands() override;
    void setCallback(fsm::StateMachine& stateMachine) override;
    void startLongPoll() override;
};

}; // namespace core
