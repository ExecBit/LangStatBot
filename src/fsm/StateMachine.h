#pragma once

#include "../Command.h"
#include "../Data.h"
#include "../IDataManager.h"
#include "../Message.h"
#include "../TelegramBotAdapter.h"
#include "../def.h"
#include "../logger/Logger.h"
#include "IState.h"

#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace fsm {

struct Context {
    Context(core::IBot* bot, core::Data& data, io_interface::IDataManager* dataMgr)
        : bot(bot), data(data), dataMgr(dataMgr) {}
    int monthNumber{};
    int dayNumber{};
    core::IBot* bot;
    core::Data& data;
    io_interface::IDataManager* dataMgr;
};

class StateMachine;
struct IdleState;
struct AddTimeState;
struct AddWordState;
struct ShowWordState;
struct ShowTimeState;
struct DumpDataState;
struct RecieveFileState;
struct EditTimeState;

class StateMachine {
  public:
    explicit StateMachine(core::IBot* bot, core::Data& data, io_interface::IDataManager* dataMgr)
        : context(bot, data, dataMgr) {
        SPDLOG_INFO("Init");

        setState<IdleState>();
    }

    void execute(const core::Message& message) { functionExecute(*this, message); }

    template <typename T, typename... Args> void setState(Args&&... args) {
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

    std::function<void(StateMachine&, const core::Message&)> functionExecute;

  private:
    std::unique_ptr<IState> state;
};

// ================== States ==================

struct IdleState : IState {
    static constexpr std::string_view _name = "Idle";
    std::string_view name() const override { return _name; }

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& msg) {
            return onMessage(stateMachine, msg);
        };
    }

    void onMessage(StateMachine& dialog, const core::Message& message) {
        SPDLOG_INFO("{} - onMessage", _name);

        switch (command::parse(message.text)) {
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
        case command::Type::recieveFile: {
            dialog.setState<RecieveFileState>(message);
        } break;
        case command::Type::editTime: {
            dialog.setState<EditTimeState>(message);
        } break;
        case command::Type::unknown: {
            dialog.context.bot->sendMessage(message.chat_id, "unknown command",
                                            def::KeyboardType::keyboardWithLayout);

            dialog.setState<IdleState>();
        }; break;
        }
        return;
    }
};

struct AddWordState : IState {
    static constexpr std::string_view _name = "AddWord";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    AddWordState(const core::Message& message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        dialog.context.bot->sendMessage(initMessage.chat_id, "Enter text", def::KeyboardType::none);
        dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
            return onMessage(stateMachine, message);
        };
    }

    void onMessage(StateMachine& dialog, const core::Message& message) {
        SPDLOG_INFO("{} - onMessage", _name);

        SPDLOG_INFO("Add word: {}", message.text);
        dialog.context.data.stat->words.push_back(message.text);
        dialog.context.bot->sendMessage(message.chat_id, "ok",
                                        def::KeyboardType::keyboardWithLayout);

        dialog.setState<IdleState>();
    }
};

struct ShowWordState : IState {
    static constexpr std::string_view _name = "ShowWord";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    ShowWordState(const core::Message& message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        if (dialog.context.data.stat->words.empty()) {
            dialog.context.bot->sendMessage(initMessage.chat_id, "Dictionary is empty",
                                            def::KeyboardType::keyboardWithLayout);
            return;
        }

        std::string msg;
        for (const auto& word : dialog.context.data.stat->words) {
            msg += word + '\n';
        }
        dialog.context.bot->sendMessage(initMessage.chat_id, msg,
                                        def::KeyboardType::keyboardWithLayout);

        dialog.setState<IdleState>();
    }
};

struct AddTimeState : IState {
    static constexpr std::string_view _name = "AddTime";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    AddTimeState(const core::Message& message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        dialog.context.bot->sendMessage(initMessage.chat_id, "choose mount of minutes",
                                        def::KeyboardType::none);

        dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
            return onWaitingMinutes(stateMachine, message);
        };
    }

    void onWaitingMinutes(StateMachine& dialog, const core::Message& message) {
        SPDLOG_INFO("{} - onWaitingMinutes", _name);

        auto now = std::chrono::system_clock::now();
        auto today = std::chrono::floor<std::chrono::days>(now);
        auto ymd = std::chrono::year_month_day(today);

        int year = static_cast<int>(ymd.year());
        unsigned month = static_cast<unsigned>(ymd.month());
        unsigned day = static_cast<unsigned>(ymd.day());

        auto& dialogContext = dialog.context;
        SPDLOG_INFO("Add time {} to date {}.{}", message.text, month, day);

        // auto& minutesCount =
        // dialogContext.data.stat->years.at(year).at(month).readDays().at(day);
        auto minutesCount = std::chrono::minutes{};
        const auto& map = dialogContext.data.stat->years.at(year).at(month).readDays();
        if (const auto itDay = map.find(day); itDay != map.end()) {
            minutesCount = itDay->second;
        }

        // = dialogContext.data.stat->years.at(year).at(month).readDays().find(day);
        const auto sourceTime = std::to_string(minutesCount.count());

        const auto currMinutes = dialogContext.data.stat->years.at(year).at(month).add(
            day, std::chrono::minutes{stoi(message.text)});

        // minutesCount += std::chrono::minutes{stoi(message->text)};
        const auto res = "Value is added, current state - " + std::to_string(day) + " : " +
                         std::to_string(currMinutes.count()) + " (" + sourceTime + " + " +
                         message.text + ")";

        dialogContext.bot->sendMessage(message.chat_id, res, def::KeyboardType::keyboardWithLayout);
        dialog.setState<IdleState>();
    }
};

struct ShowTimeState : IState {
    static constexpr std::string_view _name = "ShowTime";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    ShowTimeState(const core::Message& message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        if (!dialog.context.data.stat) {
            dialog.context.bot->sendMessage(initMessage.chat_id, "Sheduler is empty",
                                            def::KeyboardType::keyboardWithLayout);
            dialog.setState<IdleState>();
            return;
        }

        dialog.context.bot->sendMessage(initMessage.chat_id, "Enter month number",
                                        def::KeyboardType::keyboardChooseMonth);
        dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
            return onWaitingMonthNumber(stateMachine, message);
        };
    }

    void onWaitingMonthNumber(StateMachine& dialog, const core::Message& message) {
        SPDLOG_INFO("{} - onWaitingMonthNumber", _name);
        SPDLOG_INFO("Got Month number {}", message.text);

        auto& dialogContext = dialog.context;
        const auto& monthStat = dialogContext.data.stat->years.at(2025).at(stoi(message.text));

        dialogContext.bot->sendMessage(message.chat_id, monthStat.print(),
                                       def::KeyboardType::keyboardWithLayout);

        dialog.setState<IdleState>();
    }
};

struct DumpDataState : IState {
    static constexpr std::string_view _name = "DumpData";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    DumpDataState(const core::Message& message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        auto& dialogContext = dialog.context;

        if (const auto& rawString = dialogContext.dataMgr->save(dialogContext.data, "./data.json");
            rawString.empty()) {
            dialogContext.bot->sendMessage(initMessage.chat_id, "error",
                                           def::KeyboardType::keyboardWithLayout);
        } else {
            dialogContext.bot->sendMessage(initMessage.chat_id, "success",
                                           def::KeyboardType::keyboardWithLayout);
        }

        dialog.setState<IdleState>();
    }
};

struct RecieveFileState : IState {
    static constexpr std::string_view _name = "RecieveFile";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    RecieveFileState(const core::Message& message) : initMessage(message) {}

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

        dialogContext.bot->sendDocument(
            initMessage.chat_id,
            {.data = "error", .mimeType = "application/json", .fileName = "data.json"},
            def::KeyboardType::none);

        dialog.setState<IdleState>();
    }
};

struct EditTimeState : IState {
    static constexpr std::string_view _name = "AddTime";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    EditTimeState(const core::Message& message) : initMessage(message) {}

    void onEnter(StateMachine& dialog) override {
        SPDLOG_INFO("{} - onEnter", _name);

        dialog.context.bot->sendMessage(initMessage.chat_id, "Enter month number",
                                        def::KeyboardType::keyboardChooseMonth);

        dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
            return onWaitingMonthNumber(stateMachine, message);
        };
    }

    void onWaitingMonthNumber(StateMachine& dialog, const core::Message& message) {
        SPDLOG_INFO("{} - onWaitingMonthNumber", _name);

        SPDLOG_INFO("Add month number: {}", message.text);
        dialog.context.bot->sendMessage(message.chat_id, "choose day", def::KeyboardType::none);

        dialog.context.monthNumber = stoi(message.text);

        dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
            return onWaitingDayNumber(stateMachine, message);
        };
    }

    void onWaitingDayNumber(StateMachine& dialog, const core::Message& message) {
        SPDLOG_INFO("{} - onWaitingDayNumber", _name);
        SPDLOG_INFO("Add day: {}", message.text);

        dialog.context.bot->sendMessage(message.chat_id, "choose mount of minutes",
                                        def::KeyboardType::none);
        dialog.context.dayNumber = stoi(message.text);

        dialog.functionExecute = [this](StateMachine& stateMachine, const core::Message& message) {
            return onWaitingMinutes(stateMachine, message);
        };
    }

    void onWaitingMinutes(StateMachine& dialog, const core::Message& message) {
        SPDLOG_INFO("{} - onWaitingMinutes", _name);
        auto& dialogContext = dialog.context;
        SPDLOG_INFO("Add time {} to date {}.{}", message.text, dialogContext.monthNumber,
                    dialogContext.dayNumber);

        dialogContext.data.stat->years.at(2025)
            .at(dialogContext.monthNumber)
            .edit(dialogContext.dayNumber, std::chrono::minutes{stoi(message.text)});

        //      dialogContext.data.stat->years.at(2025).at(dialogContext.monthNumber).days[dialogContext.dayNumber]
        //      =
        //          std::chrono::minutes{stoi(message->text)};

        std::string res{"Value is added, current state: "};
        res += std::to_string(dialogContext.dayNumber) + " : " + message.text;
        dialogContext.bot->sendMessage(message.chat_id, res, def::KeyboardType::keyboardWithLayout);
        dialog.setState<IdleState>();
    }
};

} // namespace fsm
