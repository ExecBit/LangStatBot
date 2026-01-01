#include <spdlog/spdlog.h>

#include <cstdlib>
#include <memory>
#include <string>

#include "core/App.h"
#include "io_interface/DataManager.h"
#include "io_interface/Storage.h"
#include "serialization/JsonSerializer.h"
#include "logger/Logger.h"

namespace io_interface {
class DataManager;
};

int main() {
    logger::init();

    std::string token;
    if (const auto env = getenv("TOKEN"); !env) {
        SPDLOG_ERROR("FATAL! TOKEN not found");
        return 1;
    } else {
        token = std::string{env};
    }

    std::string dataPath;
//  if (const auto env = getenv("DATA_PATH"); !env) {
//      SPDLOG_ERROR("FATAL! DATA_PATH not found");
//      return 1;
//  } else {
//      dataPath = std::string{env};
//  }

    auto dataMgr = std::make_unique<io_interface::DataManager>(std::make_unique<io_interface::Storage>(),
                                                               std::make_unique<serialization::JsonSerializer>(), dataPath);

    core::App app{token, std::move(dataMgr)};

    if (!app.init("./data.json")) {
        SPDLOG_ERROR("FATAL");
        return 1;
    }

    app.start();

    SPDLOG_INFO("APP FINISH");

    return 0;
}
