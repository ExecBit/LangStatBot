#pragma once

#include "fsm/IState.h"
#include "Message.h"

namespace fsm {

struct IdleState : IState {
    static constexpr std::string_view _name = "Idle";
    std::string_view name() const override { return _name; }

    void onEnter(StateMachine& dialog) override;
    void onMessage(StateMachine& dialog, const core::Message& message);
};

}; // namespace fsm
