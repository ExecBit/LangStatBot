#pragma once

#include "fsm/IState.h"
#include "core/Message.h"

namespace fsm {

struct TotalStatState : IState {
    static constexpr std::string_view _name = "TotalStat";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    TotalStatState(const core::Message& message);

    void onEnter(StateMachine& dialog) override;
};

}; // namespace fsm
