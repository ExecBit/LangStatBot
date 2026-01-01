#pragma once
#include <cstdint>
#include <string>

namespace core {

struct Message {
    int64_t chat_id;
    std::string text;
};

} // namespace core
