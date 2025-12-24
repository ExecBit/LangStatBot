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

bool DataManager::save(const core::Data& src, std::string_view path) {
    std::string rawData;
    if (!m_serializer->serialize(src, rawData)) {
        return false;
    }

    if (!m_storage->write(path, rawData)) {
        return false;
    }

    return true;
}

};  // namespace io_interface
