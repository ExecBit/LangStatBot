#pragma once

#include "core/Data.h"
#include "io_interface/IDataManager.h"
#include "core/Message.h"
#include "IState.h"
#include "core/IBot.h"
#include "logger/Logger.h"

#include <memory>
#include <functional>

namespace fsm {

struct Context {
    Context(core::IBot* bot, core::Data& data, io_interface::IDataManager* dataMgr)
        : bot(bot), data(data), dataMgr(dataMgr) {}
    int monthNumber{};
    int dayNumber{};
    int yearNumber{};
    core::IBot* bot;
    core::Data& data;
    io_interface::IDataManager* dataMgr;
};

class StateMachine {
  public:
    explicit StateMachine(core::IBot* bot, core::Data& data, io_interface::IDataManager* dataMgr);
    void execute(const core::Message& message);

    template <typename T, typename... Args> void setState(Args&&... args) {
        static_assert(std::is_base_of_v<IState, T>, "State must inherit from IState");

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


} // namespace fsm
