#pragma once

#include "IDataManager.h"
#include "json/json.h"

namespace io_interface {

class DataManager : public IDataManager {
public:
    bool load(std::string_view path) override;
    bool save(std::string_view file, std::string_view path) override;
    bool fill(core::Data& data) override;

private:
    Json::Value m_root;
};

};  // namespace io_interface
