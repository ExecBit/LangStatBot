#include "Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace logger {

static std::shared_ptr<spdlog::logger> g_logger;

void init() {
    // g_logger = spdlog::basic_logger_mt("app", "app.log");
    // g_logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

    g_logger = spdlog::stdout_color_mt("app");
    //g_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

    g_logger->set_pattern(
        "[%Y-%m-%d %H:%M:%S.%e] "
        "[%^%l%$] "
        "[%s:%#] "
        "%v");
}

spdlog::logger& logger() { return *g_logger; }

}  // namespace logger
