#pragma once

#include "fsm/IState.h"
#include "core/Message.h"

namespace fsm {

struct RecieveFileState : IState {
    static constexpr std::string_view _name = "RecieveFile";
    std::string_view name() const override { return _name; }

    core::Message initMessage;
    RecieveFileState(const core::Message& message);

    void onEnter(StateMachine& dialog) override;
};

}; // namespace fsm
