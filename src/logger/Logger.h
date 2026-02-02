#pragma once
#include <spdlog/spdlog.h>

namespace logger {

void init(bool log_to_file);
spdlog::logger& logger();

}
