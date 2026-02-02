#include "App.h"

#include "logger/Logger.h"

namespace {
volatile std::sig_atomic_t gStopProceedLoop{0};
}
void sigint_handler(int signal) { gStopProceedLoop = signal; }

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

    SPDLOG_INFO("APP STARTED");

    while (!gStopProceedLoop) try {
        m_bot.startPoll();
    } catch (std::exception& e) {
        SPDLOG_ERROR("EXCEPTION: {}", e.what());
        SPDLOG_ERROR("RESTART");
    } catch (...) {
        SPDLOG_ERROR("FATAL EXCEPTION!");
        SPDLOG_ERROR("RESTART");
    }

    m_dataMgr->save(m_data, "./data.json");
    SPDLOG_INFO("APP FINISHED");
}

}; // namespace core
