#include "core/App.h"
#include "io_interface/DataManager.h"
#include "io_interface/Storage.h"
#include "logger/Logger.h"
#include "serialization/JsonSerializer.h"

#include <cstdlib>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace io_interface {
class DataManager;
};

void print_help(const char* prog) {
    std::cout << "Usage: " << prog << " [options]\n\n"
              << "Options:\n"
              << "  -h, --help        Show this help message\n"
              << "  --log-file        Enable logging to file (app.log)\n";
}

bool parse_args(int argc, char* argv[]) {
    bool target{false};

    for (int i = 1; i < argc; ++i) {
        std::string_view arg{argv[i]};

        if (arg == "-h" || arg == "--help") {
            print_help(argv[0]);
            std::exit(0);
        }

        if (arg == "--log-file") {
            target = true;
        }
    }

    return target;
}

int main(int argc, char** argv) {
    const auto log_to_file = parse_args(argc, argv);
    logger::init(log_to_file);

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

    auto dataMgr = std::make_unique<io_interface::DataManager>(
        std::make_unique<io_interface::Storage>(),
        std::make_unique<serialization::JsonSerializer>(), dataPath);

    core::App app{token, std::move(dataMgr)};

    if (!app.init("./data.json")) {
        SPDLOG_ERROR("FATAL");
        return 1;
    }

    app.start();

    return 0;
}
