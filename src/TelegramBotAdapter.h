#pragma once

#include "CommandBuilder.h"
#include "IBot.h"
#include "Message.h"
#include "def.h"
#include "fsm/StateMachine.h"

#include <memory>

namespace core {

class TelegramBotAdapter : public IBot {
    TgBot::Bot m_bot;
    TgBot::ReplyKeyboardMarkup::Ptr keyboardWithLayout;
    TgBot::ReplyKeyboardMarkup::Ptr keyboardChooseMonth;

  public:
    TelegramBotAdapter(const std::string& token) : m_bot(token) {

        keyboardWithLayout = std::make_shared<TgBot::ReplyKeyboardMarkup>();
        keyboardChooseMonth = std::make_shared<TgBot::ReplyKeyboardMarkup>();

        def::createKeyboard(def::keyboardWithLayoutVector, keyboardWithLayout);
        def::createKeyboard(def::keyboardChooseMonthVector, keyboardChooseMonth);
    }
    void sendMessage(std::int64_t chatId, const std::string& text,
                     def::KeyboardType keyboardType) override {
        switch (keyboardType) {
        case def::KeyboardType::keyboardChooseMonth: {
            m_bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboardChooseMonth);
        }; break;
        case def::KeyboardType::keyboardWithLayout: {
            m_bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboardWithLayout);
        }; break;
        case def::KeyboardType::none: {
            m_bot.getApi().sendMessage(chatId, text);
        }; break;
        }
    }

    void sendDocument(std::int64_t chatId, const def::DocumentData& doc,
                      def::KeyboardType) override {

        auto input_file(std::make_shared<TgBot::InputFile>());
        input_file->data = doc.data;
        input_file->mimeType = doc.mimeType;
        input_file->fileName = doc.fileName;

        m_bot.getApi().sendDocument(chatId, input_file);
    }

    void deleteWebhook() override { m_bot.getApi().deleteWebhook(); }
    void deleteMyCommands() override { m_bot.getApi().deleteMyCommands(); }
    std::string username() override { return m_bot.getApi().getMe()->username; }
    void setCommands() override {

        CommandBuilder builder{m_bot};

        builder
            .setCommand("ping",
                        [&bot = m_bot,
                         &keyboardWithLayout = keyboardWithLayout](TgBot::Message::Ptr message) {
                            bot.getApi().sendMessage(message->chat->id, "pong", nullptr, nullptr,
                                                     keyboardWithLayout);
                        })
            .setCommand("start",
                        [&bot = m_bot,
                         &keyboardWithLayout = keyboardWithLayout](TgBot::Message::Ptr message) {
                            bot.getApi().sendMessage(message->chat->id, "choose option", nullptr,
                                                     nullptr, keyboardWithLayout);
                        })
            .build();
    }

    void setCallback(fsm::StateMachine& stateMachine) override {
        m_bot.getEvents().onNonCommandMessage([&](TgBot::Message::Ptr message) {
            Message msg{.chat_id = message->chat->id, .text = message->text};
            stateMachine.execute(msg);
            // m_handler.execute(msg);
        });
    }

    void startLongPoll() override {
        static TgBot::TgLongPoll longpoll(m_bot);
        longpoll.start();
    }
};

}; // namespace core
