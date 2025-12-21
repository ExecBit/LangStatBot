#include "DataManager.h"

#include <fstream>
#include <iostream>

#include "json/json.h"

namespace io_interface {

bool DataManager::load(std::string_view path) {
    Json::Value root;
    std::ifstream ifs{path.data()};

    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        std::cout << errs << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << root << std::endl;
    return EXIT_SUCCESS;
}

bool DataManager::save(std::string_view file, std::string_view path) {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    std::ofstream ofs{path.data()};
    root["Data"] = file.data();
    writer->write(root, &ofs);

    return EXIT_SUCCESS;
}

};  // namespace io_interface
