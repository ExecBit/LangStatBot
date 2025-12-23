#include "App.h"

namespace {
volatile std::sig_atomic_t gStopProceedLoop{0};
}
void sigint_handler(int signal) {
    printf("STOP LOOP\n");
    gStopProceedLoop = signal;
}

namespace core {

bool App::init(std::string_view path) {
    if (!m_bot.initBot()) {
        return false;
    };

    if (!m_dataMgr->load(path)) {
        return false;
    }

    if (!m_dataMgr->fill(m_data)) {
        return false;
    }

    return true;
}

void App::start() {
    try {
        auto longPoll = m_bot.longPollObj();
        while (!gStopProceedLoop) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("EXCEPTION: %s\n", e.what());
    } catch (...) {
        printf("FATAL EXCEPTION!\n");
    }
}

};  // namespace core
