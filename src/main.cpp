#include <json/json.h>
#include <tgbot/tgbot.h>

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

#include "App.h"
#include "DataManager.h"
#include "IStorage.h"
#include "Storage.h"
#include "serialization/JsonSerializer.h"

namespace io_interface {
class DataManager;
};

int main() {
    const auto sl = std::source_location::current();
    std::string token;
    if (const auto env = getenv("TOKEN"); !env) {
        std::cout << "FATAL! Token not found\n";
        return 1;
    } else {
        token = std::string{env};
    }
    printf("Token: %s\n", token.c_str());

    auto dataMgr = std::make_unique<io_interface::DataManager>(std::make_unique<io_interface::Storage>(),
                                                               std::make_unique<serialization::JsonSerializer>());

    core::App app{token, std::move(dataMgr)};

    if (!app.init("./data.json")) {
        std::cout << "FATAL!\n";

        std::cerr << "Error: " << sl.file_name() << ":" << sl.line() << "'\n";
        return 1;
    }

    app.start();

    return 0;
}
