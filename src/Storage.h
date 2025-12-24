#pragma once

#include "IStorage.h"

namespace io_interface {

class Storage : public IStorage {
public:
    bool read(std::string_view path, std::string& file) override;
    bool write(std::string_view path, std::string_view file) override;
};

};  // namespace io_interface
