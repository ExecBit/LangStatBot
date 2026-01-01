#pragma once

#include "fsm/IState.h"
#include "Message.h"

namespace fsm {

struct AddTimeState : IState {
    static constexpr std::string_view _name = "AddTime";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    AddTimeState(const core::Message& message);

    void onEnter(StateMachine& dialog) override;

    void onWaitingMinutes(StateMachine& dialog, const core::Message& message);
};

}; // namespace fsm
