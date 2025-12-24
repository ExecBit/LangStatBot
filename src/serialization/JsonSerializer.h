#pragma once

#include "ISerializer.h"

namespace serialization {

class JsonSerializer : public ISerializer {
public:
    bool serialize(const core::Data& src, std::string& dst) const override;
    bool deserialize(const std::string& src, core::Data& dst) const override;
};

};  // namespace serialization
