#pragma once

#include <string_view>

namespace io_interface {

class IDataManager {
public:
    virtual ~IDataManager() = default;

    virtual bool load(std::string_view path) = 0;
    virtual bool save(std::string_view file, std::string_view path) = 0;
};

};  // namespace io_interface
