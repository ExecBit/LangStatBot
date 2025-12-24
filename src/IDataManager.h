#pragma once

#include <string_view>
#include "Data.h"

namespace io_interface {

class IDataManager {
public:
    virtual ~IDataManager() = default;

    virtual bool load(std::string_view src, core::Data& dst) = 0;
    virtual bool save(const core::Data& src, std::string_view path) = 0;
};

};  // namespace io_interface
