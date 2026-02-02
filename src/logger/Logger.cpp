#include "Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace logger {

std::shared_ptr<spdlog::logger> g_logger;

void init(bool log_to_file) {
    std::vector<spdlog::sink_ptr> sinks;

    if (log_to_file) {
        auto file_sink =
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>("app.log",
                                                                   100 * 1024 * 1024, // 5 MB
                                                                   5                  // 5 файлов
            );

        //      auto file_sink =
        //          std::make_shared<spdlog::sinks::basic_file_sink_mt>("app.log", true /* truncate
        //          */);
        sinks.push_back(file_sink);
    } else {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.push_back(console_sink);
    }

    g_logger = std::make_shared<spdlog::logger>("app", sinks.begin(), sinks.end());

    g_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] "
                          "[%^%l%$] "
                          "[%s:%#] "
                          "%v");

    spdlog::set_default_logger(g_logger);

    SPDLOG_INFO("=== log file opened ===");
}

spdlog::logger& logger() { return *g_logger; }

} // namespace logger
