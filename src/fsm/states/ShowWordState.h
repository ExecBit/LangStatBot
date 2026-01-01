#pragma once

#include "fsm/IState.h"
#include "Message.h"

namespace fsm {

struct ShowWordState : IState {
    static constexpr std::string_view _name = "ShowWord";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    ShowWordState(const core::Message& message);

    void onEnter(StateMachine& dialog) override;
};


}; // namespace fsm
