#pragma once

#include "IDataManager.h"

namespace io_interface {

class DataManager : public IDataManager {
public:
    bool load(std::string_view path);
    bool save(std::string_view file, std::string_view path);
};

};  // namespace io_interface
