#pragma once

#include <string>
#include <tgbot/tgbot.h>

namespace fsm {

class StateMachine;

struct IState {
    virtual ~IState() = default;
    virtual void onEnter(StateMachine&) = 0;
};

};

