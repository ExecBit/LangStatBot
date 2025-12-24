#include "App.h"
#include "logger/Logger.h"

namespace {
volatile std::sig_atomic_t gStopProceedLoop{0};
}
void sigint_handler(int signal) {
    gStopProceedLoop = signal;
}

namespace core {

bool App::init(std::string_view path) {
    if (!m_bot.initBot()) {
        return false;
    };

    if (!m_dataMgr->load(path, m_data)) {
        return false;
    }

    return true;
}

void App::start() {
    std::signal(SIGINT, sigint_handler);

    try {
        auto longPoll = m_bot.longPollObj();
        while (!gStopProceedLoop) {
            SPDLOG_INFO("Long poll started");
            longPoll.start();
        }
        SPDLOG_WARN("STOP LOOP");

        m_dataMgr->save(m_data, "./data.json");
    } catch (std::exception& e) {
        SPDLOG_ERROR("EXCEPTION: %s\n", e.what());
    } catch (...) {
        SPDLOG_ERROR("FATAL EXCEPTION!");
    }
}

};  // namespace core
