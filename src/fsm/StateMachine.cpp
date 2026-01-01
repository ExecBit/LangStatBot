#include "StateMachine.h"
#include "states/IdleState.h"

namespace fsm {

StateMachine::StateMachine(core::IBot* bot, core::Data& data, io_interface::IDataManager* dataMgr)
    : context(bot, data, dataMgr) {
    SPDLOG_INFO("Init");

    setState<IdleState>();
}

void StateMachine::execute(const core::Message& message) { functionExecute(*this, message); }

}; // namespace fsm
