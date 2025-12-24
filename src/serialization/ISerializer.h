#pragma once

#include "../Data.h"

namespace serialization {

class ISerializer {
public:
    virtual ~ISerializer() = default;

    virtual bool serialize(const core::Data& src, std::string& dst) const = 0;
    virtual bool deserialize(const std::string& src, core::Data& dst) const = 0;
};

};  // namespace serialization
