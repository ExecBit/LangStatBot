#pragma once

#include "fsm/IState.h"
#include "core/Message.h"

namespace fsm {

struct ShowTimeState : IState {
    static constexpr std::string_view _name = "ShowTime";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    ShowTimeState(const core::Message& message);

    void onEnter(StateMachine& dialog) override;
    void onWaitingMonthNumber(StateMachine& dialog, const core::Message& message);
    void onWaitingYearNumber(StateMachine& dialog, const core::Message& message);
};

}; // namespace fsm
