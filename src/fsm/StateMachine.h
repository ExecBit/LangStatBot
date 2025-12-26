#pragma once

#include <tgbot/tgbot.h>

#include <functional>
#include <iostream>
#include <memory>

#include "../Command.h"
#include "../Data.h"
#include "../logger/Logger.h"
#include "IState.h"

namespace {

using namespace std;
using namespace TgBot;

void createKeyboard(const vector<vector<string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb) {
    for (size_t i = 0; i < buttonLayout.size(); ++i) {
        vector<KeyboardButton::Ptr> row;
        for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
            KeyboardButton::Ptr button(new KeyboardButton);
            button->text = buttonLayout[i][j];
            button->requestContact = false;
            button->requestLocation = false;
            row.push_back(button);
        }
        kb->keyboard.push_back(row);
    }
}

}  // namespace

namespace fsm {

struct Context {
    Context(TgBot::Bot& bot, core::Data& data) : bot(bot), data(data) {}
    int monthNumber{};
    int dayNumber{};
    TgBot::Bot& bot;
    core::Data& data;
    ReplyKeyboardMarkup::Ptr keyboardWithLayout;
    ReplyKeyboardMarkup::Ptr keyboardChooseMonth;
};

class StateMachine;
struct IdleState;
struct AddTimeState;
struct AddWordState;
struct ShowWordState;
struct ShowTimeState;

class StateMachine {
public:
    explicit StateMachine(TgBot::Bot& bot, core::Data& data) : context(bot, data) {
        context.keyboardWithLayout = ReplyKeyboardMarkup::Ptr(new ReplyKeyboardMarkup);
        context.keyboardChooseMonth = ReplyKeyboardMarkup::Ptr(new ReplyKeyboardMarkup);

        createKeyboard({{"add word", "add time"}, {"show words", "show time"}}, context.keyboardWithLayout);
        createKeyboard(
            {
                {"1", "2", "3", "4", "5", "6"},
                {"7", "8", "9", "10", "11", "12"},
            },
            context.keyboardChooseMonth);

        setState<IdleState>();
    }

    void execute(TgBot::Message::Ptr message) { functionExecute(*this, message); }

    template <typename T, typename... Args>
    void setState(Args&&... args) {
        static_assert(std::is_base_of_v<IState, T>, "State must inherit from IDialogState");
        state = std::make_unique<T>(std::forward<Args>(args)...);
        state->onEnter(*this);
    }

    Context context;

    std::function<void(StateMachine&, TgBot::Message::Ptr)> functionExecute;

private:
    std::unique_ptr<IState> state;
};

// ================== States ==================

struct IdleState : IState {
    void onEnter(StateMachine& dialog) override {
        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onMessage(stateMachine, msg);
        };
    }

    void onMessage(StateMachine& dialog, TgBot::Message::Ptr message) {
        if (const auto command = command::parse(message->text); command) {
            switch (command.value()) {
                case command::Type::addWord: {
                    dialog.setState<AddWordState>(message);
                }; break;
                case command::Type::showWord: {
                    dialog.setState<ShowWordState>(message);
                }; break;
                case command::Type::addTime: {
                    dialog.setState<AddTimeState>(message);
                }; break;
                case command::Type::showTime: {
                    dialog.setState<ShowTimeState>(message);
                } break;
                case command::Type::unknown: {
                    dialog.context.bot.getApi().sendMessage(message->chat->id, "unknown command", nullptr, nullptr,
                                                            dialog.context.keyboardWithLayout);
                }; break;
            }
            return;
        }
    }
};

struct AddWordState : IState {
    TgBot::Message::Ptr initMessage;
    AddWordState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        dialog.context.bot.getApi().sendMessage(initMessage->chat->id, "Enter text");
        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onMessage(stateMachine, msg);
        };
    }

    void onMessage(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("Add word: {}", message->text);
        dialog.context.data.stat->words.push_back(message->text);
        dialog.context.bot.getApi().sendMessage(message->chat->id, "ok", nullptr, nullptr,
                                                dialog.context.keyboardWithLayout);

        dialog.setState<IdleState>();
    }
};

struct ShowWordState : IState {
    TgBot::Message::Ptr initMessage;
    ShowWordState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        if (dialog.context.data.stat->words.empty()) {
            dialog.context.bot.getApi().sendMessage(initMessage->chat->id, "Dictionary is empty", nullptr, nullptr,
                                                    dialog.context.keyboardWithLayout);
            return;
        }

        std::string msg;
        for (const auto& word : dialog.context.data.stat->words) {
            msg += word + '\n';
        }
        dialog.context.bot.getApi().sendMessage(initMessage->chat->id, msg, nullptr, nullptr,
                                                dialog.context.keyboardWithLayout);

        dialog.setState<IdleState>();
    }
};

struct AddTimeState : IState {
    TgBot::Message::Ptr initMessage;
    AddTimeState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        dialog.context.bot.getApi().sendMessage(initMessage->chat->id, "Enter month number", nullptr, nullptr,
                                                dialog.context.keyboardChooseMonth);

        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onWaitingMonthNumber(stateMachine, msg);
        };
    }

    void onWaitingMonthNumber(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("Add month number: {}", message->text);
        dialog.context.bot.getApi().sendMessage(message->chat->id, "choose day");

        dialog.context.monthNumber = stoi(message->text);

        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onWaitingDayNumber(stateMachine, msg);
        };
    }

    void onWaitingDayNumber(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("Add day: {}", message->text);

        dialog.context.bot.getApi().sendMessage(message->chat->id, "choose mount of minutes");
        dialog.context.dayNumber = stoi(message->text);

        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onWaitingMinutes(stateMachine, msg);
        };
    }

    void onWaitingMinutes(StateMachine& dialog, TgBot::Message::Ptr message) {
        auto& dialogContext = dialog.context;
        SPDLOG_INFO("Add time {} to date {}.{}", message->text, dialogContext.monthNumber, dialogContext.dayNumber);

        dialogContext.data.stat->years.at(2025).at(dialogContext.monthNumber).days[dialogContext.dayNumber] +=
            stoi(message->text);

        std::string res{"Value is added, current state: "};
        res += std::to_string(dialogContext.dayNumber) + " : " + message->text;
        dialogContext.bot.getApi().sendMessage(message->chat->id, res, nullptr, nullptr,
                                               dialogContext.keyboardWithLayout);
        dialog.setState<IdleState>();
    }
};

struct ShowTimeState : IState {
    TgBot::Message::Ptr initMessage;
    ShowTimeState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        if (!dialog.context.data.stat) {
            dialog.context.bot.getApi().sendMessage(initMessage->chat->id, "Sheduler is empty", nullptr, nullptr,
                                                    dialog.context.keyboardWithLayout);
            dialog.setState<IdleState>();
            return;
        }

        dialog.context.bot.getApi().sendMessage(initMessage->chat->id, "Enter month number", nullptr, nullptr,
                                                dialog.context.keyboardChooseMonth);
        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onWaitingMonthNumber(stateMachine, msg);
        };
    }

    void onWaitingMonthNumber(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("Got Month number {}", message->text);

        auto& dialogContext = dialog.context;
        const auto& monthStat = dialogContext.data.stat->years.at(2025).at(stoi(message->text));

        dialogContext.bot.getApi().sendMessage(message->chat->id, monthStat.print(), nullptr, nullptr,
                                               dialogContext.keyboardWithLayout);

        dialog.setState<IdleState>();
    }
};

}  // namespace fsm
