#pragma once

#include "fsm/IState.h"
#include "Message.h"

namespace fsm {

struct DumpDataState : IState {
    static constexpr std::string_view _name = "DumpData";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    DumpDataState(const core::Message& message);

    void onEnter(StateMachine& dialog) override;
};

}; // namespace fsm
