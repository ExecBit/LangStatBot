#include <json/json.h>
#include <spdlog/spdlog.h>
#include <tgbot/tgbot.h>

#include <cstdlib>
#include <memory>
#include <string>

#include "App.h"
#include "DataManager.h"
#include "Storage.h"
#include "serialization/JsonSerializer.h"
//#include "logger/LoggerMacros.h"
#include "logger/Logger.h"

namespace io_interface {
class DataManager;
};

int main() {
    logger::init();

    std::string token;
    if (const auto env = getenv("TOKEN"); !env) {
        SPDLOG_ERROR("FATAL! Token not found");
        return 1;
    } else {
        token = std::string{env};
    }

    SPDLOG_INFO("Token: {}", token);
    //SPDLOG_INFO("Token: {}", token);

    auto dataMgr = std::make_unique<io_interface::DataManager>(std::make_unique<io_interface::Storage>(),
                                                               std::make_unique<serialization::JsonSerializer>());

    core::App app{token, std::move(dataMgr)};

    if (!app.init("./data.json")) {
        SPDLOG_ERROR("FATAL");
        return 1;
    }

    app.start();

    return 0;
}
