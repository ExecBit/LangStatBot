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

    try {
        SPDLOG_INFO("Long poll started");
        while (!gStopProceedLoop) {
            m_bot.startPoll();
        }
        SPDLOG_WARN("Long poll stopped");
        m_dataMgr->save(m_data, "./data.json");
    } catch (std::exception& e) {
        SPDLOG_ERROR("EXCEPTION: {}", e.what());
    } catch (...) {
        SPDLOG_ERROR("FATAL EXCEPTION!");
    }
}

}; // namespace core
