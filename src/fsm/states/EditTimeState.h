#pragma once

#include "fsm/IState.h"
#include "core/Message.h"

namespace fsm {

struct EditTimeState : IState {
    static constexpr std::string_view _name = "AddTime";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    EditTimeState(const core::Message& message);

    void onEnter(StateMachine& dialog) override;
    void onWaitingMonthNumber(StateMachine& dialog, const core::Message& message);
    void onWaitingDayNumber(StateMachine& dialog, const core::Message& message);
    void onWaitingMinutes(StateMachine& dialog, const core::Message& message);
};

}; // namespace fsm
