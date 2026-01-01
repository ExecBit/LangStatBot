#include "Storage.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include "logger/Logger.h"

namespace io_interface {

bool Storage::read(std::string_view path, std::string& file) {
    SPDLOG_INFO("Start read file");

    std::ifstream input{path.data()};
    if (!input) {
        std::ofstream output{path.data()};
        if (!output) {
            SPDLOG_ERROR("Error, can't create file '{}'", path);
            return false;
        }
        output << "{fuckyou}\n";
        output.close();

        input = std::ifstream{path.data()};
        if (!input) {
            SPDLOG_ERROR("Error: file is created, but cannot be readed");
            return false;
        }
    }

    std::stringstream ss;
    ss << input.rdbuf();
    file.clear();
    file.append(ss.str());

    SPDLOG_INFO("read data is succeess");
    return true;
}

bool Storage::write(std::string_view path, std::string_view file) {
    SPDLOG_INFO("Start write file");

    std::ofstream output{path.data()};
    if (!output) {
        SPDLOG_ERROR("Error, can't create file '{}'", path);
        return false;
    }
    output.clear();
    output << file;

    SPDLOG_INFO("write data is succeess");
    return true;
}

};  // namespace io_interface
