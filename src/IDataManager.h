#pragma once

#include <string_view>
#include "Data.h"

namespace io_interface {

class IDataManager {
public:
    virtual ~IDataManager() = default;

    virtual bool load(std::string_view path) = 0;
    virtual bool save(std::string_view file, std::string_view path) = 0;
    virtual bool fill(core::Data& data) = 0;
};

};  // namespace io_interface
