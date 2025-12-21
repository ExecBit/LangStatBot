#include "App.h"

namespace {
volatile std::sig_atomic_t gStopProceedLoop{0};
}
void sigint_handler(int signal) { gStopProceedLoop = signal; }

namespace core {

bool App::initBot() {
    return m_bot.initBot();
}

void App::start() {
    try {
        auto longPoll = m_bot.longPollObj();
        while (!gStopProceedLoop) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    } catch (...) {
        printf("FATAL!\n");
    }
}

};  // namespace core
