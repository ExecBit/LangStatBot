#include "TelegramBotAdapter.h"

namespace core {

TelegramBotAdapter::TelegramBotAdapter(const std::string& token) : m_bot(token) {

    keyboardChooseCommands = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    keyboardChooseMonth = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    keyboardChooseYear = std::make_shared<TgBot::ReplyKeyboardMarkup>();

    def::createKeyboard(def::keyboardChooseCommandsVector, keyboardChooseCommands);
    def::createKeyboard(def::keyboardChooseMonthVector, keyboardChooseMonth);
    def::createKeyboard(def::keyboardChooseYearVector, keyboardChooseYear);
}

void TelegramBotAdapter::sendMessage(std::int64_t chatId, const std::string& text,
                                     def::KeyboardType keyboardType) {
    switch (keyboardType) {
    case def::KeyboardType::keyboardChooseMonth: {
        m_bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboardChooseMonth);
    }; break;
    case def::KeyboardType::keyboardChooseCommands: {
        m_bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboardChooseCommands);
    }; break;
    case def::KeyboardType::keyboardChooseYear: {
        m_bot.getApi().sendMessage(chatId, text, nullptr, nullptr, keyboardChooseYear);
    }; break;
    case def::KeyboardType::none: {
        m_bot.getApi().sendMessage(chatId, text);
    }; break;
    }
}

void TelegramBotAdapter::sendDocument(std::int64_t chatId, const def::DocumentData& doc,
                                      def::KeyboardType) {

    auto input_file(std::make_shared<TgBot::InputFile>());
    input_file->data = doc.data;
    input_file->mimeType = doc.mimeType;
    input_file->fileName = doc.fileName;

    m_bot.getApi().sendDocument(chatId, input_file);
}

void TelegramBotAdapter::deleteWebhook() { m_bot.getApi().deleteWebhook(); }

void TelegramBotAdapter::deleteMyCommands() { m_bot.getApi().deleteMyCommands(); }

std::string TelegramBotAdapter::username() { return m_bot.getApi().getMe()->username; }

void TelegramBotAdapter::setCommands() {

    CommandBuilder builder{m_bot};

    builder
        .setCommand("ping",
                    [&bot = m_bot,
                     &keyboardWithLayout = keyboardChooseCommands](TgBot::Message::Ptr message) {
                        bot.getApi().sendMessage(message->chat->id, "pong", nullptr, nullptr,
                                                 keyboardWithLayout);
                    })
        .setCommand("start",
                    [&bot = m_bot,
                     &keyboardWithLayout = keyboardChooseCommands](TgBot::Message::Ptr message) {
                        bot.getApi().sendMessage(message->chat->id, "choose option", nullptr,
                                                 nullptr, keyboardWithLayout);
                    })
        .build();
}

void TelegramBotAdapter::setCallback(fsm::StateMachine& stateMachine) {
    m_bot.getEvents().onNonCommandMessage([&](TgBot::Message::Ptr message) {
        Message msg{.chat_id = message->chat->id, .text = message->text};
        stateMachine.execute(msg);
    });
}

void TelegramBotAdapter::startLongPoll() {
    static TgBot::TgLongPoll longpoll(m_bot);
    longpoll.start();
}

}; // namespace core
