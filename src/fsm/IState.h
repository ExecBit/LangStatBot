#pragma once

#include <string_view>

namespace fsm {

class StateMachine;

struct IState {
    virtual ~IState() = default;
    virtual void onEnter(StateMachine&) = 0;
    virtual std::string_view name() const = 0;
};

};

