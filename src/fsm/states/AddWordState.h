#pragma once

#include "fsm/IState.h"
#include "core/Message.h"

namespace fsm {

struct AddWordState : IState {
    static constexpr std::string_view _name = "AddWord";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    AddWordState(const core::Message& message);

    void onEnter(StateMachine& dialog) override;
    void onMessage(StateMachine& dialog, const core::Message& message);
};


}; // namespace fsm
