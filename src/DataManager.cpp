#include "DataManager.h"

#include <memory>

#include "Data.h"
#include "logger/Logger.h"

namespace io_interface {

bool DataManager::load(std::string_view src, core::Data& dst) {
    SPDLOG_INFO("Start load data");
    std::string rawData;
    if (!m_storage->read(src, rawData)) {
        return false;
    }

    core::Data data;
    if (!m_serializer->deserialize(rawData, data)) {
        return false;
    }

    for (auto& stat : data.stat->years | std::views::values | std::views::join | std::views::values) {
        stat.update();
    }

    dst = std::move(data);

    SPDLOG_INFO("Load data succeess");
    return true;
}

std::string DataManager::save(const core::Data& src, std::string_view path) {
    SPDLOG_INFO("Start save data");
    std::string rawData;
    if (!m_serializer->serialize(src, rawData)) {
        return {};
    }

    if (!m_storage->write(path, rawData)) {
        return {};
    }

    SPDLOG_INFO("Save data succeess");
    return rawData;
}

};  // namespace io_interface
