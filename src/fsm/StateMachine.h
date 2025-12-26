#pragma once

#include <tgbot/tgbot.h>

#include <functional>
#include <iostream>
#include <memory>

#include "../Command.h"
#include "../Data.h"
#include "../IDataManager.h"
#include "../def.h"
#include "../logger/Logger.h"
#include "IState.h"

namespace fsm {

struct Context {
    Context(TgBot::Bot& bot, core::Data& data, io_interface::IDataManager* dataMgr)
        : bot(bot)
        , data(data)
        , dataMgr(dataMgr) {}
    int monthNumber{};
    int dayNumber{};
    TgBot::Bot& bot;
    core::Data& data;
    io_interface::IDataManager* dataMgr;
    TgBot::ReplyKeyboardMarkup::Ptr keyboardWithLayout;
    TgBot::ReplyKeyboardMarkup::Ptr keyboardChooseMonth;
};

class StateMachine;
struct IdleState;
struct AddTimeState;
struct AddWordState;
struct ShowWordState;
struct ShowTimeState;
struct DumpDataState;

class StateMachine {
public:
    explicit StateMachine(TgBot::Bot& bot, core::Data& data, io_interface::IDataManager* dataMgr)
        : context(bot, data, dataMgr) {
        SPDLOG_INFO("Init");

        context.keyboardWithLayout = TgBot::ReplyKeyboardMarkup::Ptr(new TgBot::ReplyKeyboardMarkup);
        context.keyboardChooseMonth = TgBot::ReplyKeyboardMarkup::Ptr(new TgBot::ReplyKeyboardMarkup);

        def::createKeyboard(def::keyboardWithLayoutVector, context.keyboardWithLayout);
        def::createKeyboard(def::keyboardChooseMonthVector, context.keyboardChooseMonth);

        setState<IdleState>();
    }

    void execute(TgBot::Message::Ptr message) { functionExecute(*this, message); }

    template <typename T, typename... Args>
    void setState(Args&&... args) {
        static_assert(std::is_base_of_v<IState, T>, "State must inherit from IDialogState");

        std::string_view prevState = "Initial";
        if (state) {
            prevState = state->name();
        }

        state = std::make_unique<T>(std::forward<Args>(args)...);
        state->onEnter(*this);
        SPDLOG_INFO("Transition from '{}' to '{}'", prevState, state->name());
    }

    Context context;

    std::function<void(StateMachine&, TgBot::Message::Ptr)> functionExecute;

private:
    std::unique_ptr<IState> state;
};

// ================== States ==================

struct IdleState : IState {
    static constexpr std::string_view _name = "Idle";
    std::string_view name() const override { return _name; }

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onMessage(stateMachine, msg);
        };
    }

    void onMessage(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("{} - onMessage", _name);

        switch (command::parse(message->text)) {
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
            case command::Type::dumpData: {
                dialog.setState<DumpDataState>(message);
            } break;
            case command::Type::unknown: {
                dialog.context.bot.getApi().sendMessage(message->chat->id, "unknown command", nullptr, nullptr,
                                                        dialog.context.keyboardWithLayout);

                dialog.setState<IdleState>();
            }; break;
        }
        return;
    }
};

struct AddWordState : IState {
    static constexpr std::string_view _name = "AddWord";
    std::string_view name() const override { return _name; }

    TgBot::Message::Ptr initMessage;
    AddWordState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        dialog.context.bot.getApi().sendMessage(initMessage->chat->id, "Enter text");
        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onMessage(stateMachine, msg);
        };
    }

    void onMessage(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("{} - onMessage", _name);

        SPDLOG_INFO("Add word: {}", message->text);
        dialog.context.data.stat->words.push_back(message->text);
        dialog.context.bot.getApi().sendMessage(message->chat->id, "ok", nullptr, nullptr,
                                                dialog.context.keyboardWithLayout);

        dialog.setState<IdleState>();
    }
};

struct ShowWordState : IState {
    static constexpr std::string_view _name = "ShowWord";
    std::string_view name() const override { return _name; }

    TgBot::Message::Ptr initMessage;
    ShowWordState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

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
    static constexpr std::string_view _name = "AddTime";
    std::string_view name() const override { return _name; }

    TgBot::Message::Ptr initMessage;
    AddTimeState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        dialog.context.bot.getApi().sendMessage(initMessage->chat->id, "Enter month number", nullptr, nullptr,
                                                dialog.context.keyboardChooseMonth);

        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onWaitingMonthNumber(stateMachine, msg);
        };
    }

    void onWaitingMonthNumber(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("{} - onWaitingMonthNumber", _name);

        SPDLOG_INFO("Add month number: {}", message->text);
        dialog.context.bot.getApi().sendMessage(message->chat->id, "choose day");

        dialog.context.monthNumber = stoi(message->text);

        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onWaitingDayNumber(stateMachine, msg);
        };
    }

    void onWaitingDayNumber(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("{} - onWaitingDayNumber", _name);
        SPDLOG_INFO("Add day: {}", message->text);

        dialog.context.bot.getApi().sendMessage(message->chat->id, "choose mount of minutes");
        dialog.context.dayNumber = stoi(message->text);

        dialog.functionExecute = [this](StateMachine& stateMachine, TgBot::Message::Ptr msg) {
            return onWaitingMinutes(stateMachine, msg);
        };
    }

    void onWaitingMinutes(StateMachine& dialog, TgBot::Message::Ptr message) {
        SPDLOG_INFO("{} - onWaitingMinutes", _name);
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
    static constexpr std::string_view _name = "ShowTime";
    std::string_view name() const override { return _name; }

    TgBot::Message::Ptr initMessage;
    ShowTimeState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

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
        SPDLOG_INFO("{} - onWaitingMonthNumber", _name);
        SPDLOG_INFO("Got Month number {}", message->text);

        auto& dialogContext = dialog.context;
        const auto& monthStat = dialogContext.data.stat->years.at(2025).at(stoi(message->text));

        dialogContext.bot.getApi().sendMessage(message->chat->id, monthStat.print(), nullptr, nullptr,
                                               dialogContext.keyboardWithLayout);

        dialog.setState<IdleState>();
    }
};

struct DumpDataState : IState {
    static constexpr std::string_view _name = "DumpData";
    std::string_view name() const override { return _name; }

    TgBot::Message::Ptr initMessage;
    DumpDataState(TgBot::Message::Ptr message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        auto& dialogContext = dialog.context;

        auto input_file(std::make_shared<TgBot::InputFile>());
        input_file->data = "error";
        input_file->mimeType = "application/json";
        input_file->fileName = "data.json";

        if (const auto& rawString = dialogContext.dataMgr->save(dialogContext.data, "./data.json");
            !rawString.empty()) {
            input_file->data = rawString;
        }

        // ialogContext.bot.getApi().sendDocument(initMessage->chat->id, input_file, {}, {}, nullptr,
        //                                        dialogContext.keyboardWithLayout);
        dialogContext.bot.getApi().sendDocument(initMessage->chat->id, input_file);

        dialog.setState<IdleState>();
    }
};

}  // namespace fsm
