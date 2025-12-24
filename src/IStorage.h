#pragma once

#include <string_view>

namespace io_interface {

class IStorage {
public:
    virtual ~IStorage() = default;

    virtual bool read(std::string_view path, std::string& file) = 0;
    virtual bool write(std::string_view path, std::string_view file) = 0;
};

};  // namespace io_interface
