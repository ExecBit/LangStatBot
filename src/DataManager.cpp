#include "DataManager.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <source_location>

#include "Data.h"
#include "MonthData.h"

namespace io_interface {

bool DataManager::load(std::string_view src, core::Data& dst) {
    std::string rawData;
    if (!m_storage->read(src, rawData)) {
        return false;
    }

    core::Data data;
    if (!m_serializer->deserialize(rawData, data)) {
        return false;
    }

    dst = std::move(data);

    printf("Load data succeess\n");
    return true;
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
